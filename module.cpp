#include <Python.h>
#include "MurmurHash3.h"
#include "version.py"

static PyObject *hash32(PyObject *, PyObject *);
static PyObject *hash128(PyObject *, PyObject *);
static PyObject *hash128_64(PyObject *, PyObject *);

const char *documentation =
    "Python wrapper around the MurmurHash3 library, written in C++. "
    "Functions included are: hash32, hash128, and hash128_64. All of "
    "these functions take the same parameters, which are: "
    "(<str/unicode/buffer>, [seed]), but may return different types.";

static PyMethodDef methods[] = {
    {"hash32", hash32, METH_VARARGS,
        "Calculate Murmur3 32-bit unsigned hash value. "
        "Parameters: <str>, [seed]"},
        
    {"hash128", hash128, METH_VARARGS,
        "Calculate Murmur3 128-bit hash to four 32bit integers. "
        "Returns tuple (int, int, int, int). "
        "Parameters: <str>, [seed]"},
        
    {"hash128_64", hash128_64, METH_VARARGS,
        "Calculate Murmur3 128-bit hash to two 64bit integers. "
        "Returns tuple (int, int). "
        "Parameters: <str>, [seed]"},
        
    {NULL, NULL, 0, NULL}
};

static PyObject *hash32(PyObject *self, PyObject *args)
{
    char *value = NULL;
    int len = 0;
    uint32_t seed = 0;
    uint32_t h = 0;
    PyObject *result;

    if (PyTuple_Size(args) == 2) {
        if (!PyArg_ParseTuple(args, "s#I", &value, &len, &seed)) {
            return NULL;
        }
    } else {
        if (!PyArg_ParseTuple(args, "s#", &value, &len)) {
            return NULL;
        }
    }

    MurmurHash3_x86_32(value, len, seed, &h);
    result = PyLong_FromUnsignedLong(h);
    if (!result) {
        PyErr_SetString(PyExc_MemoryError, "PyLong_FromUnsignedLong() returned NULL.");
    }

    return result;
}

static PyObject *hash128(PyObject *self, PyObject *args)
{
    char *value = NULL;
    int len = 0;
    int i;
    uint32_t seed = 0;
    uint32_t out[4] = {0, 0, 0, 0};
    PyObject *result;
    PyObject *tmp[4];

    if (PyTuple_Size(args) == 2) {
        if (!PyArg_ParseTuple(args, "s#I", &value, &len, &seed)) {
            return NULL;
        }
    } else {
        if (!PyArg_ParseTuple(args, "s#", &value, &len)) {
            return NULL;
        }
    }
    
    MurmurHash3_x86_128(value, len, seed, &out);
    
    result = PyTuple_New(4);
    if (!result) {
        PyErr_SetString(PyExc_MemoryError, "PyTuple_New() returned NULL.");
        return NULL;
    }
    
    for (i = 0; i < 4; i++) {
        tmp[i] = PyLong_FromUnsignedLong(out[i]);
        if (!tmp[i]) {
            Py_XDECREF(result);
            PyErr_SetString(PyExc_MemoryError, "PyLong_FromUnsignedLong() returned NULL.");
            return NULL;
        }
    
        PyTuple_SetItem(result, i, tmp[i]);
    }
    
    return result;
}

static PyObject *hash128_64(PyObject *self, PyObject *args)
{
    char *value = NULL;
    int len = 0;
    int i;
    uint32_t seed = 0;
    uint64_t out[2] = {0, 0};
    PyObject *result;
    PyObject *tmp[4];

    if (PyTuple_Size(args) == 2) {
        if (!PyArg_ParseTuple(args, "s#I", &value, &len, &seed)) {
            return NULL;
        }
    } else {
        if (!PyArg_ParseTuple(args, "s#", &value, &len)) {
            return NULL;
        }
    }
    
    MurmurHash3_x64_128(value, len, seed, &out);
    
    result = PyTuple_New(2);
    if (!result) {
        PyErr_SetString(PyExc_MemoryError, "PyTuple_New() returned NULL.");
        return NULL;
    }
    
    for (i = 0; i < 2; i++) {
        tmp[i] = PyLong_FromUnsignedLongLong(out[i]);
        if (!tmp[i]) {
            
            Py_XDECREF(result);
            PyErr_SetString(PyExc_MemoryError, "PyLong_FromUnsignedLong() returned NULL.");
            return NULL;
        }
    
        PyTuple_SetItem(result, i, tmp[i]);
    }
    
    return result;
}

extern "C" {
#if PY_MAJOR_VERSION <= 2
/* Python 1.x/2.x */
    PyMODINIT_FUNC
    initmurmur3(void)
    {
        //Py_Initialize();
        PyObject *module;
        
        module = Py_InitModule3("murmur3", methods, (char *)documentation);
        if (!module) return;
        
        PyModule_AddStringConstant(module, "__version__", MODULE_VERSION);
    }
#else
    /* Python 3.x */

    static PyModuleDef murmur3_module = {
        PyModuleDef_HEAD_INIT,
        "murmur3",
        documentation,
        -1,
        methods,
        NULL,
        NULL,
        NULL,
        NULL
    };

    PyMODINIT_FUNC
    PyInit_murmur3(void)
    {
        PyObject *module;
    
        module = PyModule_Create(&murmur3_module);
        if (!module) return module;
        PyModule_AddStringConstant(module, "__version__", MODULE_VERSION);
        
        return module;
    }
#endif
}
