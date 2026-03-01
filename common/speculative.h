#pragma once

#include "cheese.h"
#include "common.h"

struct common_speculative;

// comma separated list of all types
std::string common_speculative_type_name_str();

// convert string to type
enum common_speculative_type common_speculative_type_from_name(const std::string & name);

// convert type to string
std::string common_speculative_type_to_str(enum common_speculative_type type);

// check if the cheese_context is compatible for speculative decoding
// note: clears the memory of the context
bool common_speculative_is_compat(cheese_context * ctx_tgt);

common_speculative * common_speculative_init(
        common_params_speculative & params,
        cheese_context             * ctx_tgt);

void common_speculative_free(common_speculative * spec);

// optionally call once at the beginning of a new generation
void common_speculative_begin(common_speculative * spec, const cheese_tokens & prompt);

// sample up to n_draft tokens and add them to the batch using the draft model
cheese_tokens common_speculative_draft(
                     common_speculative * spec,
        const common_params_speculative & params,
                     const cheese_tokens & prompt,
                            cheese_token   id_last);

// informs the speculative decoder that n_accepted tokens were accepted by the target model
void common_speculative_accept(common_speculative * spec, uint16_t n_accepted);

// print statistics about the speculative decoding
void common_speculative_print_stats(const common_speculative * spec);
