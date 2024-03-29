# 2024-01-31 Profiling conversion between NumPy ndarrays and OIFArrayF64 data structures

Using the script `examples/compare_performance_ivp_burgers_eq_pure_comparison.py one scipy_ode_dopri5`, we get the following the following results:

Profile the code with the following command:
```
python -m cProfile -o profiler-results \
    examples/compare_performance_ivp_burgers_eq_pure_comparison.py \
        one scipy_ode_dopri5
```

then using
```
python -m pstats profiler-results
sort cumulative
stats 20
```

we obtain the following results:
```
Wed Jan 31 16:54:54 2024    profiler-results

         4223975 function calls (4196427 primitive calls) in 8.631 seconds

   Ordered by: cumulative time
   List reduced from 5448 to 20 due to restriction <20>

   ncalls  tottime  percall  cumtime  percall filename:lineno(function)
    744/1    0.011    0.000    8.632    8.632 {built-in method builtins.exec}
        1    0.000    0.000    8.632    8.632 examples/compare_performance_ivp_burgers_eq_pure_comparison.py:1(<module>)
        1    0.000    0.000    7.706    7.706 examples/compare_performance_ivp_burgers_eq_pure_comparison.py:177(run_one_impl)
        1    0.005    0.005    7.706    7.706 examples/compare_performance_ivp_burgers_eq_pure_comparison.py:252(_run_once)
     2000    0.005    0.000    7.378    0.004 /home/dima/sw/mambaforge/envs/um02-toy-example/lib/python3.12/site-packages/scipy/integrate/_ode.py:397(integrate)
     2000    0.497    0.000    7.373    0.004 /home/dima/sw/mambaforge/envs/um02-toy-example/lib/python3.12/site-packages/scipy/integrate/_ode.py:1173(run)
    89871    0.268    0.000    6.875    0.000 examples/compare_performance_ivp_burgers_eq_pure_comparison.py:125(compute)
    89871    0.087    0.000    3.945    0.000 examples/compare_performance_ivp_burgers_eq_pure_comparison.py:112(compute_rhs_native)
    89871    3.024    0.000    3.858    0.000 examples/compare_performance_ivp_burgers_eq_pure_comparison.py:94(compute_rhs)
    89871    1.218    0.000    1.874    0.000 examples/compare_performance_ivp_burgers_eq_pure_comparison.py:134(_c_args_from_py_args)
   826/10    0.005    0.000    1.034    0.103 <frozen importlib._bootstrap>:1349(_find_and_load)
   821/10    0.004    0.000    1.033    0.103 <frozen importlib._bootstrap>:1304(_find_and_load_unlocked)
   784/11    0.003    0.000    1.031    0.094 <frozen importlib._bootstrap>:911(_load_unlocked)
   671/11    0.002    0.000    1.031    0.094 <frozen importlib._bootstrap_external>:988(exec_module)
  1958/21    0.002    0.000    1.028    0.049 <frozen importlib._bootstrap>:480(_call_with_frames_removed)
  1006/97    0.002    0.000    0.911    0.009 <frozen importlib._bootstrap>:1390(_handle_fromlist)
   659/71    0.001    0.000    0.906    0.013 {built-in method builtins.__import__}
    90031    0.123    0.000    0.814    0.000 /home/dima/sw/mambaforge/envs/um02-toy-example/lib/python3.12/site-packages/numpy/core/fromnumeric.py:2692(max)
    89871    0.376    0.000    0.753    0.000 examples/compare_performance_ivp_burgers_eq_pure_comparison.py:154(_py_args_from_c_args)
    90230    0.210    0.000    0.692    0.000 /home/dima/sw/mambaforge/envs/um02-toy-example/lib/python3.12/site-packages/numpy/core/fromnumeric.py:71(_wrapreduction)
```

where we can see that inside the function `compute` that runs for 6.875
seconds,
the functions `_c_args_from_py_args` and `_py_args_from_c_args` take
$1.874 + 0.753 = 2.627$ seconds, which constitutes 38 % of the run time
for the function `compute` (useful work is done inside the function
`compute_rhs_native` during the other 62 % of run time).

## Line profiling

Line profiling was done with
```shell
    kernprof -l -v examples/compare_performance_ivp_burgers_eq_pure_comparison.py one scipy_ode_dopri5
```
with functions `_c_args_from_py_args` and `_py_args_from_c_args` annotated
(decorator `@profile` from `line_profiler` module).

The results are:
```
================================================================
Solving Burgers' equation with time integration scipy_ode_dopri5
Finished
Wrote profile results to compare_performance_ivp_burgers_eq_pure_comparison.py.lprof
Timer unit: 1e-06 s

Total time: 2.43264 s
File: examples/compare_performance_ivp_burgers_eq_pure_comparison.py
Function: _c_args_from_py_args at line 135

Line #      Hits         Time  Per Hit   % Time  Line Contents
==============================================================
   135                                               @profile
   136                                               def _c_args_from_py_args(self, *args) -> list:
   137     89871      56466.2      0.6      2.3          begin = time.time()
   138     89871      33840.0      0.4      1.4          c_args: list[CtypesType] = []
   139    269613     245270.1      0.9     10.1          for i, (t, v) in enumerate(zip(self.arg_types, args)):
   140    179742      55548.1      0.3      2.3              if t == OIF_INT:
   141                                                           c_args.append(ctypes.c_int(v))
   142    179742      52412.0      0.3      2.2              elif t == OIF_FLOAT64:
   143     89871      76238.6      0.8      3.1                  c_args.append(ctypes.c_double(v))
   144     89871      28563.6      0.3      1.2              elif t == OIF_ARRAY_F64:
   145     89871     118409.0      1.3      4.9                  assert v.dtype == np.float64
   146     89871      29576.1      0.3      1.2                  nd = v.ndim
   147     89871     194035.4      2.2      8.0                  dimensions = (ctypes.c_long * len(v.shape))(*v.shape)
   148     89871      42022.9      0.5      1.7                  double_p_t = ctypes.POINTER(ctypes.c_double)
   149     89871    1001718.9     11.1     41.2                  data = v.ctypes.data_as(double_p_t)
   150
   151     89871     270073.7      3.0     11.1                  oif_array = OIFArrayF64(nd, dimensions, data)
   152     89871     109151.4      1.2      4.5                  c_args.append(ctypes.pointer(oif_array))
   153     89871      37293.8      0.4      1.5          end = time.time()
   154                                                   # print("{:35s} {:.2e}".format("Elapsed time _c_args_from_py_args", end - begin))
   155     89871      82017.4      0.9      3.4          return c_args

Total time: 0.901177 s
File: examples/compare_performance_ivp_burgers_eq_pure_comparison.py
Function: _py_args_from_c_args at line 157

Line #      Hits         Time  Per Hit   % Time  Line Contents
==============================================================
   157                                               @profile
   158                                               def _py_args_from_c_args(self, *args) -> list:
   159     89871      39935.7      0.4      4.4          begin = time.time()
   160     89871      24564.8      0.3      2.7          py_args = []
   161     89871      32901.2      0.4      3.7          py_args.append(args[0])
   162     89871      18304.0      0.2      2.0          v = args[1]
   163     89871     150222.5      1.7     16.7          arr_type = ctypes.c_double * v.contents.dimensions[0]
   164    179742      52523.6      0.3      5.8          py_args.append(
   165    179742     395453.0      2.2     43.9              np.ctypeslib.as_array(
   166     89871     107230.1      1.2     11.9                  arr_type.from_address(ctypes.addressof(v.contents.data.contents))
   167                                                       )
   168                                                   )
   175     89871      28710.8      0.3      3.2          end = time.time()
   176                                                   # print("{:35s} {:.2e}".format("Elapsed time _py_args_from_c_args", end - begin))
   177
   178     89871      51331.4      0.6      5.7          return py_args
```

We can see from the results of line profiling, that somehow functions
`ndarray.ctypes.data_as` and `numpy.ctypeslib.as_array` are taking 40 % of run
time, and look as the main targets for optimization.
