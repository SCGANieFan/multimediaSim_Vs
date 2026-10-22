#pragma once
#include "dyn_lib_loader.h"
#include "smf_dyn_api.h"

#define SMF_DYM_SYM_DEF(lib, cnt) \
                            static struct dyn_lib_info* _libInfo = 0;\
                            static void* _funcTbls[cnt];\
                            static bool _smf_load() {\
                                bool rst = smf_load_dll(#lib, true);\
                                _libInfo = dyn_lib_find_by_name(#lib);\
                                return rst && !!_libInfo;\
                            }\
                            static void _smf_unload() {\
                                memset(_funcTbls, 0, sizeof(_funcTbls));\
                                smf_unload_dll(#lib, true);\
                                _libInfo = 0;\
                            }



#define SMF_DYN_SYM(idx, ret, func, argv, argvs, err) \
                                                    ret func argv {\
                                                    typedef ret (*functype) argv;\
                                                    if(!_funcTbls[idx]) {\
                                                        void* pFunc = dyn_lib_sym(_libInfo, #func);\
                                                        if(!pFunc) return err;\
                                                        _funcTbls[idx] = pFunc;\
                                                    }\
                                                    return ((functype)_funcTbls[idx]) argvs;\
                                                    }

#define SMF_DYN_SYM_INIT(idx, ret, func, argv, argvs, err) \
													ret func argv {\
                                                    typedef ret (*functype) argv;\
                                                    if(!_smf_load()) return err; \
                                                    if(!_funcTbls[idx]) {\
                                                        void* pFunc = dyn_lib_sym(_libInfo, #func);\
                                                        if(!pFunc) return err;\
                                                        _funcTbls[idx] = pFunc;\
                                                    }\
                                                    return ((functype)_funcTbls[idx]) argvs;\
                                                    }

#define SMF_DYN_SYM_DEINIT(idx, ret, func, argv, argvs, err) \
													ret func argv {\
                                                    typedef ret (*functype) argv;\
                                                    if(!_funcTbls[idx]) {\
                                                       _funcTbls[idx] = dyn_lib_sym(_libInfo, #func);\
                                                    }\
                                                    ret res = _funcTbls[idx] ? ((functype)_funcTbls[idx]) argvs : err;\
                                                    _smf_unload();\
                                                    return res;}
