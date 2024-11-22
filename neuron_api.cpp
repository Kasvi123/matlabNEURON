// compile via: mex CXXFLAGS="-std=c++17" neuron_api.cpp
#include "mex.h"
#include <stdio.h>
#include <array>
#include <tuple>
#include <unordered_map>
#include <string>

#ifdef _WIN32
    #include "c:\nrn\include\neuronapi.h"
    #include <windows.h>
    #define DLL_HANDLE HMODULE
    #define DLL_LOAD(name) LoadLibrary(name)
    #define DLL_GET_PROC(handle, name) GetProcAddress(handle, name)
    #define DLL_FREE(handle) FreeLibrary(handle)
    #define DLL_ERROR() "Error loading library"
    const char* NEURON_LIBRARY_PATH = "c:\\nrn\\bin\\libnrniv.dll";
#else
    #include "/Users/kasvisingh/nrn/src/nrniv/neuronapi.h"
    #include <dlfcn.h>
    #define DLL_HANDLE void*
    #define DLL_LOAD(name) dlopen(name, RTLD_NOW)
    #define DLL_GET_PROC(handle, name) dlsym(handle, name)
    #define DLL_FREE(handle) dlclose(handle)
    #define DLL_ERROR() dlerror()
    const char* NEURON_LIBRARY_PATH = "/usr/local/lib/libnrniv.dylib";
#endif

typedef void (*MxFunctionPtr)(const mxArray*, mxArray*);
Section* (*nrn_section_new_)(char const* name) = nullptr;
void (*nrn_hoc_call_)(char const* command) = nullptr;
void (*nrn_double_push_)(double) = nullptr;
void (*nrn_function_call_)(char const* name, int narg) = nullptr;
void (*nrn_nseg_set_)(Section*, int) = nullptr;
void (*nrn_section_push_)(Section*) = nullptr;
void (*nrn_section_pop_)(void) = nullptr; //i added
char const* (*nrn_secname_)(Section*) = nullptr; //i added
void (*nrn_mechanism_insert_)(Section*, const Symbol*) = nullptr;
int (*nrn_nseg_get_)(Section const*) = nullptr;
void (*nrn_segment_diam_set_)(Section*, double, double) = nullptr;
double (*nrn_segment_diam_get_)(Section*, double) = nullptr;
double (*nrn_rangevar_get_)(Symbol*, Section*, double) = nullptr;
void (*nrn_rangevar_set_)(Symbol*, Section*, double, double) = nullptr;
void (*nrn_rangevar_push_)(Symbol*, Section*, double) = nullptr;
void (*nrn_section_connect_)(Section*, double, Section*, double) = nullptr;
void (*nrn_section_length_set_)(Section*, double) = nullptr;
double (*nrn_section_length_get_)(Section*) = nullptr;
double (*nrn_section_Ra_get_)(Section*) = nullptr;
void (*nrn_section_Ra_set_)(Section*, double) = nullptr;
Symbol* (*nrn_symbol_)(const char*) = nullptr;
int (*nrn_symbol_type_)(const Symbol*) = nullptr;
void (*nrn_symbol_push_)(Symbol*) = nullptr;
double (*nrn_double_pop_)(void) = nullptr;
void (*nrn_double_ptr_push_)(double*) = nullptr;
double* (*nrn_double_ptr_pop_)(void) = nullptr;
void (*nrn_str_push_)(char**) = nullptr;
char** (*nrn_pop_str_)(void) = nullptr;
void (*nrn_int_push_)(int) = nullptr;
int (*nrn_int_pop_)(void) = nullptr;
void (*nrn_object_push_)(Object*) = nullptr;
bool has_inited = false;
DLL_HANDLE neuron_handle = nullptr;
std::unordered_map<std::string, void(*)(const mxArray**, mxArray**)> function_map; 


// Print to MATLAB command window.
int mlprint(int stream, char* msg) {
    // stream = 1 for stdout; otherwise stderr
    if (stream == 1) {
        mexPrintf(msg);
    } else {
        // We could add something to error messages here.
        mexPrintf(msg);
    }
    return 0;
}

std::string getStringFromMxArray(const mxArray* mxStr) {
    if (mxGetClassID(mxStr) != mxCHAR_CLASS) {
        mexErrMsgIdAndTxt("MyModule:invalidInput", "Input must be a string.");
    }

    // Get the number of characters in the string
    mwSize length = mxGetNumberOfElements(mxStr);

    // Allocate memory for the string plus null terminator
    std::string result(length, '\0');

    // Copy the string into the allocated buffer
    mxGetString(mxStr, &result[0], length + 1);

    return result;
}


void nrn_section_new(const mxArray* prhs[], mxArray* plhs[]) {
    Section* sec = nrn_section_new_(getStringFromMxArray(prhs[1]).c_str());
    plhs[0] = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
    uint64_t* ptr = (uint64_t*)mxGetData(plhs[0]);
    *ptr = reinterpret_cast<uint64_t>(sec);
}
void nrn_hoc_call(const mxArray* prhs[], mxArray* plhs[]) {
    nrn_hoc_call_(getStringFromMxArray(prhs[1]).c_str());
}
void nrn_double_push(const mxArray* prhs[], mxArray* plhs[]) {
    nrn_double_push_(mxGetScalar(prhs[1]));
}
void nrn_nseg_set(const mxArray* prhs[], mxArray* plhs[]) {
    uint64_t* ptr = (uint64_t*)mxGetData(prhs[1]);
    Section* sec = reinterpret_cast<Section*>(*ptr);
    int nseg = (int) mxGetScalar(prhs[2]);
    nrn_nseg_set_(sec, nseg)   ;
}
void nrn_nseg_get(const mxArray* prhs[], mxArray* plhs[]) {
    uint64_t* ptr = (uint64_t*)mxGetData(prhs[1]);
    Section* sec = reinterpret_cast<Section*>(*ptr);
    int nseg = nrn_nseg_get_(sec);
    plhs[0] = mxCreateDoubleScalar(nseg);
}

void nrn_section_push(const mxArray* prhs[], mxArray* plhs[]) {
    uint64_t* ptr = (uint64_t*)mxGetData(prhs[1]);
    Section* sec = reinterpret_cast<Section*>(*ptr);
    nrn_section_push_(sec);
}

void nrn_secname(const mxArray* prhs[], mxArray* plhs[]) {
    uint64_t* ptr = (uint64_t*)mxGetData(prhs[1]);
    Section* sec = reinterpret_cast<Section*>(*ptr);
    const char* sec_name = nrn_secname_(sec);
    plhs[0] = mxCreateString(sec_name);
} 

void nrn_section_pop(const mxArray* prhs[], mxArray* plhs[]) {
    nrn_section_pop_();
}


void nrn_mechanism_insert(const mxArray* prhs[], mxArray* plhs[]) {
    uint64_t* ptr = (uint64_t*)mxGetData(prhs[1]);
    Section* sec = reinterpret_cast<Section*>(*ptr);
    Symbol* mechanism = reinterpret_cast<Symbol*>(mxGetData(prhs[2]));
    nrn_mechanism_insert_(sec, mechanism);
}

void nrn_segment_diam_set(const mxArray* prhs[], mxArray* plhs[]) {
    uint64_t* ptr = (uint64_t*)mxGetData(prhs[1]);
    Section* sec = reinterpret_cast<Section*>(*ptr);
    double x = mxGetScalar(prhs[2]);
    double diam = mxGetScalar(prhs[3]);
    nrn_segment_diam_set_(sec, x, diam);
}
!git
// this is function that created on my own for nrn segment get 
void nrn_segment_diam_get(const mxArray* prhs[], mxArray* plhs[]) {
    uint64_t* ptr = (uint64_t*)mxGetData(prhs[1]);
    Section* sec = reinterpret_cast<Section*>(*ptr);
    double x = mxGetScalar(prhs[2]);
    double diam = nrn_segment_diam_get_(sec, x);
    plhs[0] = mxCreateDoubleScalar(diam);
}

void nrn_rangevar_get(const mxArray* prhs[], mxArray* plhs[]) {
    uint64_t* ptr = (uint64_t*)mxGetData(prhs[1]);
    Section* sec = reinterpret_cast<Section*>(*ptr);
    Symbol* sym = reinterpret_cast<Symbol*>(mxGetData(prhs[2]));
    double x = mxGetScalar(prhs[3]);
    double value = nrn_rangevar_get_(sym, sec, x);
    plhs[0] = mxCreateDoubleScalar(value);
}

void nrn_rangevar_set(const mxArray* prhs[], mxArray* plhs[]) {
    uint64_t* ptr = (uint64_t*)mxGetData(prhs[1]);
    Section* sec = reinterpret_cast<Section*>(*ptr);
    Symbol* sym = reinterpret_cast<Symbol*>(mxGetData(prhs[2]));
    double x = mxGetScalar(prhs[3]);
    double value = mxGetScalar(prhs[4]);
    nrn_rangevar_set_(sym, sec, x, value);
}

void nrn_rangevar_push(const mxArray* prhs[], mxArray* plhs[]) {
    uint64_t* ptr = (uint64_t*)mxGetData(prhs[1]);
    Section* sec = reinterpret_cast<Section*>(*ptr);
    Symbol* sym = reinterpret_cast<Symbol*>(mxGetData(prhs[2]));
    double x = mxGetScalar(prhs[3]);
    nrn_rangevar_push_(sym, sec, x);
}

// Replace with the actual header file

void nrn_section_connect(const mxArray* prhs[], mxArray* plhs[]) {
    uint64_t* child_ptr = (uint64_t*)mxGetData(prhs[1]);
    uint64_t* parent_ptr = (uint64_t*)mxGetData(prhs[3]);
    Section* child_sec = reinterpret_cast<Section*>(*child_ptr);
    Section* parent_sec = reinterpret_cast<Section*>(*parent_ptr);
    if (!child_sec || !parent_sec) {
        mexErrMsgIdAndTxt("NEURON:SectionConnect", "Invalid section pointers");
    }
    double child_x = mxGetScalar(prhs[2]);
    double parent_x = mxGetScalar(prhs[4]);
    if (child_x < 0 || child_x > 1 || parent_x < 0 || parent_x > 1) {
        mexErrMsgIdAndTxt("NEURON:SectionConnect", "Connection points must be between 0 and 1");
    }
    if (child_sec == parent_sec) {
        mexErrMsgIdAndTxt("NEURON:SectionConnect", "Cannot connect a section to itself");
    }
    nrn_section_connect_(child_sec, child_x, parent_sec, parent_x);
    printf("Testing section connections:\n");
    printf("-- %s (position %.2f)\n", nrn_secname_(child_sec), child_x);
    printf("    -- %s (position %.2f)\n", nrn_secname_(parent_sec), parent_x);
}

void nrn_section_length_set(const mxArray* prhs[], mxArray* plhs[]) {
    uint64_t* ptr = (uint64_t*)mxGetData(prhs[1]);
    Section* sec = reinterpret_cast<Section*>(*ptr);
    //Section* sec = (Section*) mxGetData(prhs[1]);
    double length = mxGetScalar(prhs[2]);
    nrn_section_length_set_(sec, length);
} 

void nrn_section_length_get(const mxArray* prhs[], mxArray* plhs[]) {
    uint64_t* ptr = (uint64_t*)mxGetData(prhs[1]);
    Section* sec = reinterpret_cast<Section*>(*ptr);
    double length = nrn_section_length_get_(sec);
    plhs[0] = mxCreateDoubleScalar(length);
}

void nrn_section_Ra_get(const mxArray* prhs[], mxArray* plhs[]) {
    uint64_t* ptr = (uint64_t*)mxGetData(prhs[1]);
    Section* sec = reinterpret_cast<Section*>(*ptr);
    double Ra = nrn_section_Ra_get_(sec);
    plhs[0] = mxCreateDoubleScalar(Ra);
}

void nrn_section_Ra_set(const mxArray* prhs[], mxArray* plhs[]) {
    uint64_t* ptr = (uint64_t*)mxGetData(prhs[1]);
    Section* sec = reinterpret_cast<Section*>(*ptr);
    double val = mxGetScalar(prhs[2]);
    nrn_section_Ra_set_(sec, val);
}


void nrn_symbol(const mxArray* prhs[], mxArray* plhs[]) {
    const char* name = mxArrayToString(prhs[1]);
    Symbol* symbol = nrn_symbol_(name);
    plhs[0] = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
    *((Symbol**)mxGetData(plhs[0])) = symbol;
}

void nrn_symbol_type(const mxArray* prhs[], mxArray* plhs[]) {
    Symbol* sym = (Symbol*) mxGetData(prhs[1]);
    int type = nrn_symbol_type_(sym);
    plhs[0] = mxCreateDoubleScalar(type);
}

void nrn_symbol_push(const mxArray* prhs[], mxArray* plhs[]) {
    Symbol* sym = (Symbol*) mxGetData(prhs[1]);
    nrn_symbol_push_(sym);
}

void nrn_double_pop(const mxArray* prhs[], mxArray* plhs[]) {
    double result = nrn_double_pop_();
    plhs[0] = mxCreateDoubleScalar(result);
}

void nrn_double_ptr_push(const mxArray* prhs[], mxArray* plhs[]) {
    double* addr = (double*) mxGetData(prhs[1]);
    nrn_double_ptr_push_(addr);
}

void nrn_double_ptr_pop(const mxArray* prhs[], mxArray* plhs[]) {
    double* result = nrn_double_ptr_pop_();
    plhs[0] = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
    *((double**)mxGetData(plhs[0])) = result;
}

void nrn_str_push(const mxArray* prhs[], mxArray* plhs[]) {
    char* str = mxArrayToString(prhs[1]);
    nrn_str_push_(&str);
}

void nrn_pop_str(const mxArray* prhs[], mxArray* plhs[]) {
    char** str = nrn_pop_str_();
    plhs[0] = mxCreateString(*str);
}

void nrn_int_push(const mxArray* prhs[], mxArray* plhs[]) {
    int i = mxGetScalar(prhs[1]);
    nrn_int_push_(i);
}

void nrn_int_pop(const mxArray* prhs[], mxArray* plhs[]) {
    int result = nrn_int_pop_(); 
    plhs[0] = mxCreateDoubleScalar(result);
}

void nrn_object_push(const mxArray* prhs[], mxArray* plhs[]) {
    Object* obj = (Object*) mxGetData(prhs[1]);
    nrn_object_push_(obj);
}



/*
commented out because this is wrong: it takes a symbol* and narg
void nrn_function_call(const mxArray* prhs[], mxArray* plhs[]) {
    auto [name, narg] = extractParams<std::string, int>(prhs, 1);
    nrn_function_call_(name.c_str(), narg);
}
*/

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (!neuron_handle) {
        #ifndef _WIN32
        // Load the wrapper library first
        DLL_HANDLE wrapper_handle = DLL_LOAD("/Users/kasvisingh/Desktop/neuron-20240826/libmodlreg.dylib");
        if (!wrapper_handle) {
            mexErrMsgIdAndTxt("load_neuron:loadFailure", "Failed to load libmodlreg.dylib: %s", DLL_ERROR());
            return;
        }
        #endif
    
        // Load the NEURON library next
        neuron_handle = DLL_LOAD(NEURON_LIBRARY_PATH);
        if (!neuron_handle) {
            mexErrMsgIdAndTxt("load_neuron:loadFailure", "Failed to load libnrniv.dylib: %s", DLL_ERROR());
            //DLL_FREE(wrapper_handle);  // Clean up before returning
            //DLL_FREE(neuron_handle);
            return;
        }
    
        static std::array<const char*, 4> argv = {"hh_sim", "-nogui", "-nopython", nullptr};
        auto nrn_init = (int (*)(int, const char**)) DLL_GET_PROC(neuron_handle, "nrn_init");
    
        if (nrn_init) {
            nrn_init(3, argv.data());
        } else {
            mexErrMsgIdAndTxt("load_neuron:functionNotFound", "Function nrn_init not found: %s", DLL_ERROR());
            //DLL_FREE(wrapper_handle);
            //DLL_FREE(neuron_handle);
            return;
        }
    
        auto nrn_stdout_redirect = (void (*)(int (*)(int, char*))) DLL_GET_PROC(neuron_handle, "nrn_stdout_redirect");
        if (nrn_stdout_redirect) {
            nrn_stdout_redirect(mlprint);
        } else {
            mexErrMsgIdAndTxt("load_neuron:functionNotFound", "Function nrn_stdout_redirect not found: %s", DLL_ERROR());
            //DLL_FREE(wrapper_handle);
            //DLL_FREE(neuron_handle);
            return;
        }

        // setup the mappings
        nrn_section_new_ = (Section* (*)(char const*)) DLL_GET_PROC(neuron_handle, "nrn_section_new");
        function_map["nrn_section_new"] = nrn_section_new;
        nrn_hoc_call_ = (void(*)(const char*)) DLL_GET_PROC(neuron_handle, "nrn_hoc_call");
        function_map["nrn_hoc_call"] = nrn_hoc_call;
        nrn_double_push_ = (void(*)(double)) DLL_GET_PROC(neuron_handle, "nrn_double_push");
        function_map["nrn_double_push"] = nrn_double_push;
        nrn_nseg_set_ = (void(*)(Section*, int)) DLL_GET_PROC(neuron_handle, "nrn_nseg_set");
        function_map["nrn_nseg_set"] = nrn_nseg_set;
        nrn_nseg_get_ = (int(*)(Section const*)) DLL_GET_PROC(neuron_handle, "nrn_nseg_get");
        function_map["nrn_nseg_get"] = nrn_nseg_get;
        nrn_section_push_ = (void(*)(Section*)) DLL_GET_PROC(neuron_handle, "nrn_section_push");
        function_map["nrn_section_push"] = nrn_section_push;
        nrn_secname_ = (char const* (*)(Section*)) DLL_GET_PROC(neuron_handle, "nrn_secname");
        function_map["nrn_secname"] = nrn_secname;
        nrn_section_pop_ = (void (*)(void)) DLL_GET_PROC(neuron_handle, "nrn_section_pop");
        function_map["nrn_section_pop"] = nrn_section_pop;
        nrn_mechanism_insert_ = (void (*)(Section*, const Symbol*)) DLL_GET_PROC(neuron_handle, "nrn_mechanism_insert");
        function_map["nrn_mechanism_insert"] = nrn_mechanism_insert;
        nrn_segment_diam_set_ = (void (*)(Section*, double, double)) DLL_GET_PROC(neuron_handle, "nrn_segment_diam_set");
        function_map["nrn_segment_diam_set"] = nrn_segment_diam_set;
        nrn_segment_diam_get_ = (double (*)(Section*, double)) DLL_GET_PROC(neuron_handle, "nrn_segment_diam_get");
        function_map["nrn_segment_diam_get"] = nrn_segment_diam_get;
        nrn_rangevar_get_ = (double (*)(Symbol*, Section*, double)) DLL_GET_PROC(neuron_handle, "nrn_rangevar_get");
        function_map["nrn_rangevar_get"] = nrn_rangevar_get;
        nrn_rangevar_set_ = (void (*)(Symbol*, Section*, double, double)) DLL_GET_PROC(neuron_handle, "nrn_rangevar_set");
        function_map["nrn_rangevar_set"] = nrn_rangevar_set;
        nrn_rangevar_push_ = (void (*)(Symbol*, Section*, double)) DLL_GET_PROC(neuron_handle, "nrn_rangevar_push");
        function_map["nrn_rangevar_push"] = nrn_rangevar_push; 
        nrn_section_connect_ = (void (*)(Section*, double, Section*, double)) DLL_GET_PROC(neuron_handle, "nrn_section_connect");
        function_map["nrn_section_connect"] = nrn_section_connect;
        nrn_section_length_set_ = (void (*)(Section*, double)) DLL_GET_PROC(neuron_handle, "nrn_section_length_set");
        function_map["nrn_section_length_set"] = nrn_section_length_set;
        nrn_section_length_get_ = (double (*)(Section*)) DLL_GET_PROC(neuron_handle, "nrn_section_length_get");
        function_map["nrn_section_length_get"] = nrn_section_length_get;
        nrn_section_Ra_get_ = (double (*)(Section*)) DLL_GET_PROC(neuron_handle, "nrn_section_Ra_get");
        function_map["nrn_section_Ra_get"] = nrn_section_Ra_get;
        nrn_section_Ra_set_ = (void (*)(Section*, double)) DLL_GET_PROC(neuron_handle, "nrn_section_Ra_set");
        function_map["nrn_section_Ra_set"] = nrn_section_Ra_set;
        nrn_symbol_ = (Symbol* (*)(const char*)) DLL_GET_PROC(neuron_handle, "nrn_symbol");
        function_map["nrn_symbol"] = nrn_symbol;
        nrn_symbol_type_ = (int (*)(const Symbol*)) DLL_GET_PROC(neuron_handle, "nrn_symbol_type");
        function_map["nrn_symbol_type"] = nrn_symbol_type;
        nrn_symbol_push_ = (void (*)(Symbol*)) DLL_GET_PROC(neuron_handle, "nrn_symbol_push");
        function_map["nrn_symbol_push"] = nrn_symbol_push;
        nrn_double_pop_ = (double (*)(void)) DLL_GET_PROC(neuron_handle, "nrn_double_pop");
        function_map["nrn_double_pop"] = nrn_double_pop;
        nrn_double_ptr_push_ = (void (*)(double*)) DLL_GET_PROC(neuron_handle, "nrn_double_ptr_push");
        function_map["nrn_double_ptr_push"] = nrn_double_ptr_push;
        nrn_double_ptr_pop_ = (double* (*)(void)) DLL_GET_PROC(neuron_handle, "nrn_double_ptr_pop");
        function_map["nrn_double_ptr_pop"] = nrn_double_ptr_pop;        
        nrn_str_push_ = (void (*)(char**)) DLL_GET_PROC(neuron_handle, "nrn_str_push");
        function_map["nrn_str_push"] = nrn_str_push;        
        nrn_pop_str_ = (char** (*)(void)) DLL_GET_PROC(neuron_handle, "nrn_pop_str");
        function_map["nrn_pop_str"] = nrn_pop_str;        
        nrn_int_push_ = (void (*)(int)) DLL_GET_PROC(neuron_handle, "nrn_int_push");
        function_map["nrn_int_push"] = nrn_int_push;       
        nrn_int_pop_ = (int (*)(void)) DLL_GET_PROC(neuron_handle, "nrn_int_pop");
        function_map["nrn_int_pop"] = nrn_int_pop;
        nrn_object_push_ = (void (*)(Object*)) DLL_GET_PROC(neuron_handle, "nrn_object_push");
        function_map["nrn_object_push"] = nrn_object_push;


        

        //nrn_function_call_ = (void(*)(const char*,int)) DLL_GET_PROC(neuron_handle, "nrn_function_call");
        //function_map["nrn_function_call"] = nrn_function_call;


        // Clean up
        //DLL_FREE(wrapper_handle);
        //DLL_FREE(neuron_handle);
    }
    if (nrhs) {
        std::string name = getStringFromMxArray(prhs[0]);
        
        auto item = function_map.find(name);
        if (item != function_map.end()) {
            // call it
            item->second(prhs, plhs);
        } else {
            mexErrMsgIdAndTxt("MyModule:unknownFunction", "Function name not recognized.");
        }
    }
}

