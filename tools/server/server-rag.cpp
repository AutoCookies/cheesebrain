// RAG integration with pomaidb: CheeseEmbeddingProvider and pipeline helpers.
// Built only when CHEESE_HAVE_POMAI_RAG is defined.

#if defined(CHEESE_HAVE_POMAI_RAG)

#include "server-rag.h"
#include "server-common.h"

#include "cheese.h"
#include "common.h"
#include "chat.h"

#include "pomai/options.h"
#include "pomai/pomai.h"
#include "pomai/rag/pipeline.h"

// palloc is linked when CHEESE_HAVE_POMAI_RAG and palloc-static exist; use for future arena-backed buffers if needed.

#include <algorithm>
#include <cstring>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace server_rag {

namespace {

// Tokenize text using cheese context; add BOS/special per common_tokenize.
std::vector<cheese_token> tokenize_for_embed(cheese_context* ctx, std::string_view text) {
    std::string s(text);
    return common_tokenize(ctx, s, true, true);
}

}  // namespace

CheeseEmbeddingProvider::CheeseEmbeddingProvider(cheese_context* ctx, const cheese_model* model)
    : ctx_(ctx)
    , model_(model)
    , dim_(static_cast<uint32_t>(cheese_model_n_embd_out(model)))
{
}

CheeseEmbeddingProvider::~CheeseEmbeddingProvider() = default;

pomai::Status CheeseEmbeddingProvider::Embed(
    std::span<const std::string_view> texts,
    std::vector<std::vector<float>>* out,
    uint32_t dim)
{
    if (out == nullptr)
        return pomai::Status::InvalidArgument("out is null");
    if (dim != dim_)
        return pomai::Status::InvalidArgument("dim mismatch");
    out->clear();
    out->reserve(texts.size());

    cheese_memory_t mem = cheese_get_memory(ctx_);
    const int32_t n_embd = static_cast<int32_t>(dim_);
    cheese_set_embeddings(ctx_, true);

    for (std::string_view text : texts) {
        if (text.empty()) {
            out->push_back(std::vector<float>(dim_, 0.0f));
            continue;
        }
        std::vector<cheese_token> tokens = tokenize_for_embed(ctx_, text);
        if (tokens.empty()) {
            out->push_back(std::vector<float>(dim_, 0.0f));
            continue;
        }
        cheese_batch batch = cheese_batch_get_one(tokens.data(), static_cast<int32_t>(tokens.size()));
        int32_t ret = cheese_decode(ctx_, batch);
        if (ret != 0) {
            cheese_set_embeddings(ctx_, false);
            return pomai::Status::Internal("cheese_decode failed");
        }
        const float* emb = nullptr;
        if (cheese_pooling_type(ctx_) != CHEESE_POOLING_TYPE_NONE)
            emb = cheese_get_embeddings_seq(ctx_, 0);
        if (emb == nullptr)
            emb = cheese_get_embeddings_ith(ctx_, -1);
        if (emb == nullptr) {
            cheese_set_embeddings(ctx_, false);
            return pomai::Status::Internal("cheese_get_embeddings failed");
        }
        out->emplace_back(emb, emb + n_embd);
        if (mem)
            cheese_memory_seq_rm(mem, 0, 0, -1);
    }

    cheese_set_embeddings(ctx_, false);
    return pomai::Status::Ok();
}

bool create_rag(std::string_view membrane_name,
               const RagOptions& opts,
               CheeseEmbeddingProvider* provider,
               std::unique_ptr<pomai::DB>* out_db,
               std::unique_ptr<pomai::RagPipeline>* out_pipeline)
{
    if (!opts.enabled || opts.embedding_dim == 0 || provider == nullptr || out_db == nullptr || out_pipeline == nullptr)
        return false;

    pomai::DBOptions db_opt;
    db_opt.path = opts.path;
    db_opt.dim = opts.embedding_dim;
    db_opt.shard_count = static_cast<uint32_t>(opts.shard_count);
    db_opt.fsync = pomai::FsyncPolicy::kNever;

    std::unique_ptr<pomai::DB> db;
    pomai::Status st = pomai::DB::Open(db_opt, &db);
    if (!st.ok())
        return false;

    pomai::MembraneSpec rag_spec;
    rag_spec.name = std::string(membrane_name);
    rag_spec.dim = opts.embedding_dim;
    rag_spec.shard_count = static_cast<uint32_t>(opts.shard_count);
    rag_spec.kind = pomai::MembraneKind::kRag;

    st = db->CreateMembrane(rag_spec);
    if (!st.ok() && st.code() != pomai::ErrorCode::kAlreadyExists) {
        return false;
    }
    st = db->OpenMembrane(membrane_name);
    if (!st.ok())
        return false;

    pomai::RagPipelineOptions pipe_opts;
    pipe_opts.max_chunk_bytes = 512;
    pipe_opts.max_doc_bytes = 4 * 1024 * 1024;
    pipe_opts.max_chunks_per_batch = 32;

    *out_db = std::move(db);
    *out_pipeline = std::make_unique<pomai::RagPipeline>(
        out_db->get(),
        membrane_name,
        opts.embedding_dim,
        provider,
        pipe_opts);
    return true;
}

pomai::Status ingest_document(pomai::RagPipeline* pipeline, uint64_t doc_id, std::string_view text) {
    if (pipeline == nullptr)
        return pomai::Status::InvalidArgument("pipeline is null");
    return pipeline->IngestDocument(doc_id, text);
}

pomai::Status retrieve_context(pomai::RagPipeline* pipeline,
                               std::string_view query,
                               uint32_t top_k,
                               std::string* out_context) {
    if (pipeline == nullptr || out_context == nullptr)
        return pomai::Status::InvalidArgument("pipeline or out_context is null");
    return pipeline->RetrieveContext(query, top_k, out_context);
}

}  // namespace server_rag

#endif  // CHEESE_HAVE_POMAI_RAG
