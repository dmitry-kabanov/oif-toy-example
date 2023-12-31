#include "oif/dispatch_api.h"
#include <stdlib.h>

#include <oif/api.h>
#include <oif/dispatch.h>
#include <oif/interfaces/ivp.h>

int oif_ivp_set_rhs_fn(ImplHandle implh,
                       void (*rhs)(double t,
                                   OIFArrayF64 *y,
                                   OIFArrayF64 *y_dot)) {
    OIFCallback rhs_wrapper = {.src = OIF_LANG_C, .fn_p = NULL, .c_fn_p = &rhs};
    OIFArgType in_arg_types[] = {OIF_CALLBACK};
    void *in_arg_values[] = {&rhs_wrapper};
    OIFArgs in_args = {
        .num_args = 1,
        .arg_types = in_arg_types,
        .arg_values = in_arg_values,
    };

    OIFArgType out_arg_types[] = {};
    void *out_arg_values[] = {};
    OIFArgs out_args = {
        .num_args = 0,
        .arg_types = out_arg_types,
        .arg_values = out_arg_values,
    };

    int status =
        call_interface_method(implh, "set_rhs_fn", &in_args, &out_args);

    return status;
}

int oif_ivp_set_initial_value(ImplHandle implh, OIFArrayF64 *y0, double t0) {
    OIFArgType in_arg_types[] = {OIF_ARRAY_F64, OIF_FLOAT64};
    void *in_arg_values[] = {&y0, &t0};
    OIFArgs in_args = {
        .num_args = 2,
        .arg_types = in_arg_types,
        .arg_values = in_arg_values,
    };

    OIFArgType out_arg_types[] = {};
    void *out_arg_values[] = {};
    OIFArgs out_args = {
        .num_args = 0,
        .arg_types = out_arg_types,
        .arg_values = out_arg_values,
    };

    int status =
        call_interface_method(implh, "set_initial_value", &in_args, &out_args);

    return status;
}

int oif_ivp_integrate(ImplHandle implh, double t, OIFArrayF64 *y) {
    OIFArgType in_arg_types[] = {OIF_FLOAT64};
    void *in_arg_values[] = {&t};
    OIFArgs in_args = {
        .num_args = 1,
        .arg_types = in_arg_types,
        .arg_values = in_arg_values,
    };

    OIFArgType out_arg_types[] = {OIF_ARRAY_F64};
    void *out_arg_values[] = {&y};
    OIFArgs out_args = {
        .num_args = 1,
        .arg_types = out_arg_types,
        .arg_values = out_arg_values,
    };

    int status = call_interface_method(implh, "integrate", &in_args, &out_args);

    return status;
}
