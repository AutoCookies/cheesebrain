// Optional RAG integration with pomaidb (CHEESE_HAVE_POMAI_RAG only).
// Provides embedding provider and pipeline helpers for the server.

#pragma once

#if defined(CHEESE_HAVE_POMAI_RAG)

#include "pomai/rag/embedding_provider.h"
#include "pomai/status.h"

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

struct cheese_context;
struct cheese_model;

namespace pomai {
class DB;
class RagPipeline;
struct RagPipelineOptions;
}  // namespace pomai

namespace server_rag {

// Embedding provider that uses a cheese context for embedding texts.
// Not thread-safe: use with a dedicated embedding context.
class CheeseEmbeddingProvider : public pomai::EmbeddingProvider {
public:
    CheeseEmbeddingProvider(struct cheese_context* ctx, const struct cheese_model* model);
    ~CheeseEmbeddingProvider() override;

    pomai::Status Embed(std::span<const std::string_view> texts,
                        std::vector<std::vector<float>>* out,
                        uint32_t dim) override;
    uint32_t Dim() const override { return dim_; }

private:
    struct cheese_context* ctx_;
    const struct cheese_model* model_;
    uint32_t dim_;
};

// Options for opening RAG (path, embedding dim, etc.). Filled by server from params.
struct RagOptions {
    std::string path;
    uint32_t embedding_dim = 0;
    int shard_count = 1;
    bool enabled = false;
};

// Create DB and pipeline. Returns true on success. Call from server init when RAG enabled.
// provider must outlive the returned pipeline.
bool create_rag(std::string_view membrane_name,
                const RagOptions& opts,
                CheeseEmbeddingProvider* provider,
                std::unique_ptr<pomai::DB>* out_db,
                std::unique_ptr<pomai::RagPipeline>* out_pipeline);

// Ingest one document into the RAG pipeline.
pomai::Status ingest_document(pomai::RagPipeline* pipeline, uint64_t doc_id, std::string_view text);

// Retrieve context for a query. Returns concatenated chunk text.
pomai::Status retrieve_context(pomai::RagPipeline* pipeline,
                               std::string_view query,
                               uint32_t top_k,
                               std::string* out_context);

}  // namespace server_rag

#endif  // CHEESE_HAVE_POMAI_RAG
