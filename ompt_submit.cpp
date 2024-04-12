#include <assert.h>
#include <inttypes.h>
#include <omp.h>
#include <omp-tools.h>
#include <stdio.h>

static void on_ompt_callback_submit_emi(ompt_scope_endpoint_t endpoint,
                                        ompt_data_t *target_data,
                                        ompt_id_t *host_op_id,
                                        unsigned int requested_num_teams) {
  switch (endpoint) {
  case ompt_scope_begin:
    printf("submit_emi begin\n");
    target_data->value++;
    break;
  case ompt_scope_end:
    printf("submit_emi end\n");
    target_data->value--;
    break;
  case ompt_scope_beginend:
    printf("WARN: got ompt_scope_beginend");
    break;
  }
}

static void on_ompt_callback_submit_intel(ompt_scope_endpoint_t endpoint,
                                          ompt_id_t target_id,
                                          ompt_id_t host_op_id,
                                          unsigned int requested_num_teams) {
  switch (endpoint) {
  case ompt_scope_begin:
    printf("submit_intel begin\n");
    break;
  case ompt_scope_end:
    printf("submit_intel end\n");
    break;
  case ompt_scope_beginend:
    printf("WARN: got ompt_scope_beginend");
    break;
  }
}

static void on_ompt_callback_submit(ompt_id_t target_id,
                                    ompt_id_t host_op_id,
                                    unsigned int requested_num_teams) {
  printf("submit teams:%u\n", requested_num_teams);
}

int ompt_initialize(ompt_function_lookup_t lookup, int initial_device_num,
                    ompt_data_t *tool_data) {
  ompt_set_callback_t ompt_set_callback =
      (ompt_set_callback_t) (uintptr_t) lookup("ompt_set_callback");

  if (ompt_set_callback == NULL) {
    printf("ERR: couldn't init ompt\n");
    return 0;
  }

  /*
  const ompt_set_result_t res = ompt_set_callback(
      ompt_callback_target_submit_emi,
      (ompt_callback_t) (uintptr_t) on_ompt_callback_submit_emi
  );
  */

  const ompt_set_result_t res = ompt_set_callback(
      ompt_callback_target_submit,
      (ompt_callback_t) (uintptr_t) on_ompt_callback_submit
  );

  if (res != ompt_set_always) {
    printf("WARN: callback set result: %d\n", res);
  }

  tool_data->value = 100;
  return 1; // success
}

void ompt_finalize(ompt_data_t *tool_data) {
  printf("count: %lu\n", tool_data->value);
}

ompt_start_tool_result_t *ompt_start_tool(unsigned int omp_version,
                                          const char *runtime_version) {
  static ompt_start_tool_result_t ompt_start_tool_result = {
      &ompt_initialize, &ompt_finalize, {.value = 0}};
  printf("ompt init\n");
  return &ompt_start_tool_result;
}
