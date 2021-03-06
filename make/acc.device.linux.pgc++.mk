ACC := pgc++ CUDA_HOME=$(cuda_dir)
acc_opt_flags__ := -Mcuda=$(acc_cc_flag__)
ifeq ($(opt),debug)
   acc_opt_flags__ += -O0 -g -traceback -ta=tesla:$(acc_cc_flag__),lineinfo
else ifeq ($(opt),release)
   acc_opt_flags__ += -O3 -ta=tesla:fastmath,$(acc_cc_flag__)
else ifeq ($(opt),profile)
   acc_opt_flags__ += -O3 -g -ta=tesla:fastmath,$(acc_cc_flag__),lineinfo
endif
acc_flags__ := -std=c++11 -acc verystrict -Minfo=accel $(shared_flags__) $(acc_opt_flags__)

acc_compile_flags__ := $(acc_flags__) -c
acc_compile_flags__ += -I$(top_dir__)/include/syntax/acc

$(all_acc_objs__): %.o: $(top_dir__)/%.cpp
	$(ACC) $< $(acc_compile_flags__) -o $@
