#include <stdio.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <Python.h>


/*----------------------------------------------------------------------
     ____                 _        _                 
    |  _ \  ___   ___ ___| |_ _ __(_)_ __   __ _ ___ 
    | | | |/ _ \ / __/ __| __| '__| | '_ \ / _` / __|
    | |_| | (_) | (__\__ \ |_| |  | | | | | (_| \__ \
    |____/ \___/ \___|___/\__|_|  |_|_| |_|\__, |___/
					   |___/     
----------------------------------------------------------------------*/

// #include <docstrings.h>

#define DOCSTRING_LFO "FIXME Lorem ipsum dolor FIXME"

/*----------------------------------------------------------------------
     ____        __ _       _ _   _                 
    |  _ \  ___ / _(_)_ __ (_) |_(_) ___  _ __  ___ 
    | | | |/ _ \ |_| | '_ \| | __| |/ _ \| '_ \/ __|
    | |_| |  __/  _| | | | | | |_| | (_) | | | \__ \
    |____/ \___|_| |_|_| |_|_|\__|_|\___/|_| |_|___/
                                                
----------------------------------------------------------------------*/

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define T_FRACTION_SCALE 1000000000.0

#ifndef M_PI
# define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078
#endif

typedef struct LFO {
    PyObject_HEAD

    struct timespec t0;
    double period;
    int frozen;
    double _frozen_phase;
    int cycle;
    int cycles;
    int _cycles_left;

    double sine_attenuverter;
    double sine_offset;
    double cosine_attenuverter;
    double cosine_offset;
    double triangle_attenuverter;
    double triangle_offset;
    double sawtooth_attenuverter;
    double sawtooth_offset;
    double square_attenuverter;
    double square_offset;
    double pw;
    double pw_offset;
    double one_attenuverter;
    double one_offset;
    double zero_attenuverter;
    double zero_offset;
} LFO;

/* Utilities */
static double timespec_to_double(struct timespec *t);
static void double_to_timespec(struct timespec *t, double val);
static double diff_timespec(struct timespec *t1, struct timespec *t0);
static double get_phase(LFO *self);

static void freeze(LFO *self);
static void unfreeze(LFO *self);
static double get_t(LFO *self);
static double get_normalized(LFO *self);
static int get_cycle(LFO *self);

static double get_sine(LFO *self);
static double get_cosine(LFO *self);
static double get_triangle(LFO *self);
static double get_sawtooth(LFO *self);
static double get_square(LFO *self);
static double get_one(LFO *self);
static double get_zero(LFO *self);

static double get_inv_sine(LFO *self);
static double get_inv_cosine(LFO *self);
static double get_inv_triangle(LFO *self);
static double get_inv_sawtooth(LFO *self);
static double get_inv_square(LFO *self);
static double get_inv_one(LFO *self);
static double get_inv_zero(LFO *self);

/* Module level functions */
static PyObject * lfo_new(PyTypeObject *type, PyObject *args, PyObject *kwargs);

/* Class definition */
static PyTypeObject lfo_type;
static int lfo___init__(LFO *self, PyObject *args, PyObject *kwargs);
static void lfo_dealloc(LFO *self);

/* Class methods */
static PyObject * lfo___repr__(LFO *self);
static PyObject * lfo___call__(LFO *self);
static int lfo___bool__(LFO *self);
static PyObject * lfo___int__(LFO *self);
static PyObject * lfo___float__(LFO *self);
static PyObject * lfo___iter__(PyObject *o);
static PyObject * lfo___next__(LFO *self);
static PyObject * lfo_richcompare(PyObject *o1, PyObject *o2, int op);
static PyObject * lfo_reset(LFO *self, PyObject *args, PyObject *kwargs);
static PyObject * lfo_freeze(LFO *self);
static PyObject * lfo_unfreeze(LFO *self);
static PyObject * lfo_is_frozen(LFO *self);
static PyObject * lfo_set_attenuverters(LFO *self, PyObject *o);
static PyObject * lfo_set_offsets(LFO *self, PyObject *o);

/* Class attributes */
static PyObject * lfo_getter_period(LFO *self, void *closure);
static int lfo_setter_period(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_cycles(LFO *self, void *closure);
static int lfo_setter_cycles(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_frequency(LFO *self, void *closure);
static int lfo_setter_frequency(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_t(LFO *self, void *closure);
static PyObject * lfo_getter_normalized(LFO *self, void *closure);
static PyObject * lfo_getter_cycle(LFO *self, void *closure);
static PyObject * lfo_getter_frozen(LFO *self, void *closure);
static int lfo_setter_frozen(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_sine_attenuverter(LFO *self, void *closure);
static int lfo_setter_sine_attenuverter(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_cosine_attenuverter(LFO *self, void *closure);
static int lfo_setter_cosine_attenuverter(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_triangle_attenuverter(LFO *self, void *closure);
static int lfo_setter_triangle_attenuverter(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_sawtooth_attenuverter(LFO *self, void *closure);
static int lfo_setter_sawtooth_attenuverter(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_square_attenuverter(LFO *self, void *closure);
static int lfo_setter_square_attenuverter(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_one_attenuverter(LFO *self, void *closure);
static int lfo_setter_one_attenuverter(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_zero_attenuverter(LFO *self, void *closure);
static int lfo_setter_zero_attenuverter(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_sine_offset(LFO *self, void *closure);
static int lfo_setter_sine_offset(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_cosine_offset(LFO *self, void *closure);
static int lfo_setter_cosine_offset(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_triangle_offset(LFO *self, void *closure);
static int lfo_setter_triangle_offset(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_sawtooth_offset(LFO *self, void *closure);
static int lfo_setter_sawtooth_offset(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_square_offset(LFO *self, void *closure);
static int lfo_setter_square_offset(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_one_offset(LFO *self, void *closure);
static int lfo_setter_one_offset(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_zero_offset(LFO *self, void *closure);
static int lfo_setter_zero_offset(LFO *self, PyObject *val, void *closure);

static PyObject * lfo_getter_pw(LFO *self, void *closure);
static int lfo_setter_pw(LFO *self, PyObject *val, void *closure);
static PyObject * lfo_getter_pw_offset(LFO *self, void *closure);
static int lfo_setter_pw_offset(LFO *self, PyObject *val, void *closure);

static PyObject * lfo_getter_sine(LFO *self, void *closure);
static PyObject * lfo_getter_cosine(LFO *self, void *closure);
static PyObject * lfo_getter_triangle(LFO *self, void *closure);
static PyObject * lfo_getter_sawtooth(LFO *self, void *closure);
static PyObject * lfo_getter_square(LFO *self, void *closure);
static PyObject * lfo_getter_one(LFO *self, void *closure);
static PyObject * lfo_getter_zero(LFO *self, void *closure);

static PyObject * lfo_getter_inv_sine(LFO *self, void *closure);
static PyObject * lfo_getter_inv_cosine(LFO *self, void *closure);
static PyObject * lfo_getter_inv_triangle(LFO *self, void *closure);
static PyObject * lfo_getter_inv_sawtooth(LFO *self, void *closure);
static PyObject * lfo_getter_inv_square(LFO *self, void *closure);
static PyObject * lfo_getter_inv_one(LFO *self, void *closure);
static PyObject * lfo_getter_inv_zero(LFO *self, void *closure);

/* Module init */
PyMODINIT_FUNC PyInit__lfo(void);


/*----------------------------------------------------------------------
     _     _           _ _                 
    | |__ (_)_ __   __| (_)_ __   __ _ ___ 
    | '_ \| | '_ \ / _` | | '_ \ / _` / __|
    | |_) | | | | | (_| | | | | | (_| \__ \
    |_.__/|_|_| |_|\__,_|_|_| |_|\__, |___/
				 |___/     
----------------------------------------------------------------------*/


/* Module level methods */

/* Dunder methods */
static PyNumberMethods lfo_as_number = {
    .nb_bool = (inquiry)lfo___bool__,
    .nb_int = (unaryfunc)lfo___int__,
    .nb_float = (unaryfunc)lfo___float__,
};

/* Class level methods */
static PyMethodDef lfo_methods[] = {
    {"reset", (PyCFunction)lfo_reset, METH_NOARGS, NULL},
    {"freeze", (PyCFunction)lfo_freeze, METH_NOARGS, NULL},
    {"unfreeze", (PyCFunction)lfo_unfreeze, METH_NOARGS, NULL},
    {"is_frozen", (PyCFunction)lfo_is_frozen, METH_NOARGS, NULL},
    {"set_attenuverters", (PyCFunction)lfo_set_attenuverters, METH_O, NULL},
    {"set_offsets", (PyCFunction)lfo_set_offsets, METH_O, NULL},
    {NULL},
};


/* Properties */
static PyGetSetDef lfo_getset[] = {
    {"period", (getter)lfo_getter_period, (setter)lfo_setter_period, NULL, NULL},
    {"cycles", (getter)lfo_getter_cycles, (setter)lfo_setter_cycles, NULL, NULL},
    {"frequency", (getter)lfo_getter_frequency, (setter)lfo_setter_frequency, NULL, NULL},
    {"frozen", (getter)lfo_getter_frozen, (setter)lfo_setter_frozen, NULL, NULL},

    {"t", (getter)lfo_getter_t, NULL, NULL, NULL},
    {"normalized", (getter)lfo_getter_normalized, NULL, NULL, NULL},
    {"cycle", (getter)lfo_getter_cycle, NULL, NULL, NULL},

    {"sine", (getter)lfo_getter_sine, NULL, NULL, NULL},
    {"cosine", (getter)lfo_getter_cosine, NULL, NULL, NULL},
    {"triangle", (getter)lfo_getter_triangle, NULL, NULL, NULL},
    {"sawtooth", (getter)lfo_getter_sawtooth, NULL, NULL, NULL},
    {"square", (getter)lfo_getter_square, NULL, NULL, NULL},
    {"one", (getter)lfo_getter_one, NULL, NULL, NULL},
    {"zero", (getter)lfo_getter_zero, NULL, NULL, NULL},

    {"inv_sine", (getter)lfo_getter_inv_sine, NULL, NULL, NULL},
    {"inv_cosine", (getter)lfo_getter_inv_cosine, NULL, NULL, NULL},
    {"inv_triangle", (getter)lfo_getter_inv_triangle, NULL, NULL, NULL},
    {"inv_sawtooth", (getter)lfo_getter_inv_sawtooth, NULL, NULL, NULL},
    {"inv_square", (getter)lfo_getter_inv_square, NULL, NULL, NULL},
    {"inv_one", (getter)lfo_getter_inv_one, NULL, NULL, NULL},
    {"inv_zero", (getter)lfo_getter_inv_zero, NULL, NULL, NULL},

    {"sine_attenuverter", (getter)lfo_getter_sine_attenuverter, (setter)lfo_setter_sine_attenuverter, NULL, NULL},
    {"cosine_attenuverter", (getter)lfo_getter_cosine_attenuverter, (setter)lfo_setter_cosine_attenuverter, NULL, NULL},
    {"triangle_attenuverter", (getter)lfo_getter_triangle_attenuverter, (setter)lfo_setter_triangle_attenuverter, NULL, NULL},
    {"sawtooth_attenuverter", (getter)lfo_getter_sawtooth_attenuverter, (setter)lfo_setter_sawtooth_attenuverter, NULL, NULL},
    {"square_attenuverter", (getter)lfo_getter_square_attenuverter, (setter)lfo_setter_square_attenuverter, NULL, NULL},
    {"one_attenuverter", (getter)lfo_getter_one_attenuverter, (setter)lfo_setter_one_attenuverter, NULL, NULL},
    {"zero_attenuverter", (getter)lfo_getter_zero_attenuverter, (setter)lfo_setter_zero_attenuverter, NULL, NULL},
    {"sine_offset", (getter)lfo_getter_sine_offset, (setter)lfo_setter_sine_offset, NULL, NULL},
    {"cosine_offset", (getter)lfo_getter_cosine_offset, (setter)lfo_setter_cosine_offset, NULL, NULL},
    {"triangle_offset", (getter)lfo_getter_triangle_offset, (setter)lfo_setter_triangle_offset, NULL, NULL},
    {"sawtooth_offset", (getter)lfo_getter_sawtooth_offset, (setter)lfo_setter_sawtooth_offset, NULL, NULL},
    {"square_offset", (getter)lfo_getter_square_offset, (setter)lfo_setter_square_offset, NULL, NULL},
    {"one_offset", (getter)lfo_getter_one_offset, (setter)lfo_setter_one_offset, NULL, NULL},
    {"zero_offset", (getter)lfo_getter_zero_offset, (setter)lfo_setter_zero_offset, NULL, NULL},
    {"pw", (getter)lfo_getter_pw, (setter)lfo_setter_pw, NULL, NULL},
    {"pw_offset", (getter)lfo_getter_pw_offset, (setter)lfo_setter_pw_offset, NULL, NULL},

    {NULL},
};


static PyTypeObject lfo_type = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "_pglfo.LFO",
    .tp_doc = PyDoc_STR(DOCSTRING_LFO),
    .tp_basicsize = sizeof(LFO),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = lfo_new,
    .tp_init = (initproc)lfo___init__,
    .tp_repr = (reprfunc)lfo___repr__,
    .tp_call = (ternaryfunc)lfo___call__,
    .tp_as_number = &lfo_as_number,
    .tp_richcompare = (richcmpfunc)lfo_richcompare,
    .tp_iter = (getiterfunc)lfo___iter__,
    .tp_iternext = (iternextfunc)lfo___next__,
    /* .tp_dealloc = (destructor)lfo_dealloc, */
    /* .tp_members = lfo_members, */
    .tp_methods = lfo_methods,
    .tp_getset = lfo_getset,
};


static PyModuleDef lfo_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "_lfo",
    .m_doc = "The _lfo module that contains the LFO class",
    .m_size = -1,
};


/*----------------------------------------------------------------------
	   _   _ _ _ _   _           
     _   _| |_(_) (_) |_(_) ___  ___ 
    | | | | __| | | | __| |/ _ \/ __|
    | |_| | |_| | | | |_| |  __/\__ \
     \__,_|\__|_|_|_|\__|_|\___||___/

----------------------------------------------------------------------*/

#define is_lfo(o) (PyType_IsSubtype(Py_TYPE(o), &lfo_type))

static double timespec_to_double(struct timespec *t) {
    return t->tv_sec + t->tv_nsec / T_FRACTION_SCALE;
}


static void double_to_timespec(struct timespec *t, double val) {
    t->tv_sec = (time_t)val;
    t->tv_nsec = (long)((val - (int)val) * T_FRACTION_SCALE);
}


static double diff_timespec(struct timespec *t1, struct timespec *t0) {
    return (t1->tv_sec - t0->tv_sec) + (t1->tv_nsec - t0->tv_nsec) / T_FRACTION_SCALE;
}


static double get_phase(LFO *self) {
    if (self->frozen) return self->_frozen_phase;

    struct timespec now;
    timespec_get(&now, TIME_UTC);

    double delta = diff_timespec(&now, &self->t0);

    return fmod(delta, self->period);
}


static void freeze(LFO *self) {
    if (self->frozen) return;

    self->_frozen_phase = get_phase(self);
    self->frozen = 1;
}


static void unfreeze(LFO *self) {
    if (! self->frozen) return;

    struct timespec now;
    timespec_get(&now, TIME_UTC);

    double t0 = timespec_to_double(&now) - self->_frozen_phase;
    double_to_timespec(&self->t0, t0);

    self->_frozen_phase = 0;
    self->frozen = 0;
}


static double get_t(LFO *self) {
    if ((self->cycles == 0) || (self->cycles && get_cycle(self) < self->cycles)) {
        return get_phase(self);
    } else {
        return self->period - DBL_MIN;
    }
}


static double get_normalized(LFO *self) {
    return get_t(self) / self->period;
}


static int get_cycle(LFO *self) {
    struct timespec now;
    timespec_get(&now, TIME_UTC);

    double delta = diff_timespec(&now, &self->t0);

    return (int)(delta / self->period);
}


static double get_sine(LFO *self) {
    double t = get_normalized(self);

    return sin(t * 2 * M_PI);
}


static double get_cosine(LFO *self) {
    double t = get_normalized(self);

    return cos(t * 2 * M_PI);
}

static double get_sawtooth(LFO *self) {
    double t = get_normalized(self);

    return 1.0 - t;
}

static double get_triangle(LFO *self) {
    double t = get_normalized(self);

    if (t < 0.5) {
	return 2 * t;
    } else {
	return 1 - 2 * (t - 0.5);
    }

}

static double get_square(LFO *self) {
        double t = get_normalized(self);

        if (t >= self->pw_offset && t <= self->pw + self->pw_offset) {
            return 1;
        } else {
            return 0;
        }
    }

    static double get_one(LFO *self) {
        return 1.0;
    }

    static double get_zero(LFO *self) {
        return 0.0;
    }

    static double get_inv_sine(LFO *self) {
        return -get_sine(self);
    }

    static double get_inv_cosine(LFO *self) {
        return -get_cosine(self);
    }

    static double get_inv_triangle(LFO *self) {
        return 1 - get_triangle(self);
    }

    static double get_inv_sawtooth(LFO *self) {
        return 1 - get_sawtooth(self);
    }

    static double get_inv_square(LFO *self) {
        return 1 - get_square(self);
    }

    static double get_inv_one(LFO *self) {
        return 1 - get_one(self);
    }

    static double get_inv_zero(LFO *self) {
        return 1 - get_zero(self);
    }


    /*----------------------------------------------------------------------
          __                  _   _                 
         / _|_   _ _ __   ___| |_(_) ___  _ __  ___ 
        | |_| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
        |  _| |_| | | | | (__| |_| | (_) | | | \__ \
        |_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
                                                
    ----------------------------------------------------------------------*/

    /* None */

    /*----------------------------------------------------------------------
              _                     _       __ 
          ___| | __ _ ___ ___    __| | ___ / _|
         / __| |/ _` / __/ __|  / _` |/ _ \ |_ 
        | (__| | (_| \__ \__ \ | (_| |  __/  _|
         \___|_|\__,_|___/___/  \__,_|\___|_|  
                                           
    ----------------------------------------------------------------------*/


    static PyObject * lfo_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
        LFO *self;

        self = (LFO *)type->tp_alloc(type, 0);
        if (self != NULL) {
            /* Alloc members here if appropriate */
        }

        return (PyObject *)self;
    }


    static int lfo___init__(LFO *self, PyObject *args, PyObject *kwargs) {
        static char *kwargslist[] = {
            "period", "cycles", "frequency",
            "sine_attenuverter", "sine_offset",
            "cosine_attenuverter", "cosine_offset",
            "triangle_attenuverter", "triangle_offset",
            "sawtooth_attenuverter", "sawtooth_offset",
            "pw", "pw_offset",
            "square_attenuverter", "square_offset",
            "one_attenuverter", "one_offset",
            "zero_attenuverter", "zero_offset",
            NULL};

        timespec_get(&self->t0, TIME_UTC);
        self->period = 1.0;

        self->cycles = 0;
        self->_cycles_left = 0;

        self->frozen = 0;
        self->_frozen_phase = 0.0;

        self->sine_attenuverter = 1.0;
        self->sine_offset = 0.0;
        self->cosine_attenuverter = 1.0;
        self->cosine_offset = 0.0;
        self->triangle_attenuverter = 1.0;
        self->triangle_offset = 0.0;
        self->sawtooth_attenuverter = 1.0;
        self->sawtooth_offset = 0.0;
        self->square_attenuverter = 1.0;
        self->square_offset = 0.0;
        self->pw = 0.5;
        self->pw_offset = 0.5;
    self->one_attenuverter = 1.0;
    self->one_offset = 0.0;
    self->zero_attenuverter = 1.0;
    self->zero_offset = 0.0;

    double frequency = 0.0;

    if (!PyArg_ParseTupleAndKeywords(
		args, kwargs, "|d$iddddddddddddddddd", kwargslist,
		&self->period, &self->cycles, &frequency,
		&self->sine_attenuverter, &self->sine_offset,
		&self->cosine_attenuverter, &self->cosine_offset,
		&self->triangle_attenuverter, &self->triangle_offset,
		&self->sawtooth_attenuverter, &self->sawtooth_offset,
		&self->pw, &self->pw_offset,
		&self->square_attenuverter, &self->square_offset,
		&self->one_attenuverter, &self->one_offset,
		&self->zero_attenuverter, &self->zero_offset))
	return -1;

    if (frequency) {
        self->period = 1.0 / frequency;
    }

    return 0;
}


static void lfo_dealloc(LFO *self) {
    /* pass */
}


/*----------------------------------------------------------------------
      ____ _                     _                 _           
     / ___| | __ _ ___ ___    __| |_   _ _ __   __| | ___ _ __ 
    | |   | |/ _` / __/ __|  / _` | | | | '_ \ / _` |/ _ \ '__|
    | |___| | (_| \__ \__ \ | (_| | |_| | | | | (_| |  __/ |   
     \____|_|\__,_|___/___/  \__,_|\__,_|_| |_|\__,_|\___|_|   
                                                           
----------------------------------------------------------------------*/

static PyObject * lfo___repr__(LFO *self) {
    double t = get_phase(self);
    return PyUnicode_FromFormat(
	    "LFO(%S, cycles=%S) t=%S v=%s",
	    PyFloat_FromDouble(self->period),
	    PyFloat_FromDouble(self->cycles),
	    PyFloat_FromDouble(t),
	    PyFloat_FromDouble(get_sine(self)));
}


static PyObject * lfo___call__(LFO *self) {
    return PyFloat_FromDouble(get_sine(self));
}


static int lfo___bool__(LFO *self) {
    return get_sine(self) > 0.5;
}


static PyObject * lfo___int__(LFO *self) {
    return PyLong_FromDouble(get_sine(self) > 0.5 ? 1.0 : 0.0);
}

static PyObject * lfo___float__(LFO *self) {
    return PyFloat_FromDouble(get_sine(self));
}

static PyObject * lfo___iter__(PyObject *self) {
    Py_INCREF(self);
    return self;
}

static PyObject * lfo___next__(LFO *self) {
    if ((self->cycles == 0) || (self->cycles && get_cycle(self) < self->cycles)) {
        return PyFloat_FromDouble(get_sine(self));
    } else {
        return NULL;
    }
}

static PyObject * lfo_richcompare(PyObject *o1, PyObject *o2, int op) {
    double this;
    double other;

    if (is_lfo(o1)) {
	this = get_sine((LFO *)o1);
	other = PyFloat_AsDouble(PyNumber_Float(o2));
    } else {
	this = PyFloat_AsDouble(PyNumber_Float(o1));
	other = get_sine((LFO *)o2);
    }

    switch(op) {
	case Py_LT:
	    return PyBool_FromLong(this < other);
	    break;
	case Py_LE:
	    return PyBool_FromLong(this <= other);
	    break;
	case Py_EQ:
	    return PyBool_FromLong(this == other);
	    break;
	case Py_NE:
	    return PyBool_FromLong(this != other);
	    break;
	case Py_GT:
	    return PyBool_FromLong(this > other);
	    break;
	case Py_GE:
	    return PyBool_FromLong(this >= other);
	    break;
	default:
	    PyErr_SetString(PyExc_ValueError, "Can't convert object to number");
	    return NULL;
    }

}


/*----------------------------------------------------------------------
  ____ _                                _   _               _     
 / ___| | __ _ ___ ___   _ __ ___   ___| |_| |__   ___   __| |___ 
| |   | |/ _` / __/ __| | '_ ` _ \ / _ \ __| '_ \ / _ \ / _` / __|
| |___| | (_| \__ \__ \ | | | | | |  __/ |_| | | | (_) | (_| \__ \
 \____|_|\__,_|___/___/ |_| |_| |_|\___|\__|_| |_|\___/ \__,_|___/
                                                                  
----------------------------------------------------------------------*/

static PyObject * lfo_reset(LFO *self, PyObject *args, PyObject *kwargs) {
    timespec_get(&self->t0, TIME_UTC);

    Py_RETURN_NONE;
}


static PyObject * lfo_freeze(LFO *self) {
    freeze(self);

    Py_RETURN_NONE;
}


static PyObject * lfo_unfreeze(LFO *self) {
    unfreeze(self);

    Py_RETURN_NONE;
}


static PyObject * lfo_is_frozen(LFO *self) {
    if (self->frozen)
	Py_RETURN_TRUE;
    else
	Py_RETURN_FALSE;
}


static PyObject * lfo_set_attenuverters(LFO *self, PyObject *o) {
    double val = PyFloat_AsDouble(o);

    self->sine_attenuverter = val;
    self->cosine_attenuverter = val;
    self->triangle_attenuverter = val;
    self->sawtooth_attenuverter = val;
    self->square_attenuverter = val;
    self->one_attenuverter = val;
    self->zero_attenuverter = val;

    Py_RETURN_NONE;
}


static PyObject * lfo_set_offsets(LFO *self, PyObject *o) {
    double val = PyFloat_AsDouble(o);

    self->sine_offset = val;
    self->cosine_offset = val;
    self->triangle_offset = val;
    self->sawtooth_offset = val;
    self->square_offset = val;
    self->one_offset = val;
    self->zero_offset = val;

    Py_RETURN_NONE;
}


/*----------------------------------------------------------------------
	   _   _        _ _           _            
      __ _| |_| |_ _ __(_) |__  _   _| |_ ___  ___ 
     / _` | __| __| '__| | '_ \| | | | __/ _ \/ __|
    | (_| | |_| |_| |  | | |_) | |_| | ||  __/\__ \
     \__,_|\__|\__|_|  |_|_.__/ \__,_|\__\___||___/

----------------------------------------------------------------------*/

static PyObject * lfo_getter_sine(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_sine(self) * self->sine_attenuverter + self->sine_offset);
}


static PyObject * lfo_getter_cosine(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_cosine(self) * self->cosine_attenuverter + self->cosine_offset);
}


static PyObject * lfo_getter_triangle(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_triangle(self) * self->triangle_attenuverter + self->triangle_offset);
}


static PyObject * lfo_getter_sawtooth(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_sawtooth(self) * self->sawtooth_attenuverter + self->sawtooth_offset);
}


static PyObject * lfo_getter_square(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_square(self) * self->square_attenuverter + self->square_offset);
}


static PyObject * lfo_getter_one(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_one(self) * self->one_attenuverter + self->one_offset);
}


static PyObject * lfo_getter_zero(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_zero(self) * self->zero_attenuverter + self->zero_offset);
}


static PyObject * lfo_getter_inv_sine(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_inv_sine(self) * self->sine_attenuverter + self->sine_offset);
}


static PyObject * lfo_getter_inv_cosine(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_inv_cosine(self) * self->cosine_attenuverter + self->cosine_offset);
}


static PyObject * lfo_getter_inv_triangle(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_inv_triangle(self) * self->triangle_attenuverter + self->triangle_offset);
}


static PyObject * lfo_getter_inv_sawtooth(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_inv_sawtooth(self) * self->sawtooth_attenuverter + self->sawtooth_offset);
}


static PyObject * lfo_getter_inv_square(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_inv_square(self) * self->square_attenuverter + self->square_offset);
}


static PyObject * lfo_getter_inv_one(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_inv_one(self) * self->one_attenuverter + self->one_offset);
}


static PyObject * lfo_getter_inv_zero(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_inv_zero(self) * self->zero_attenuverter + self->zero_offset);
}

static PyObject * lfo_getter_period(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->period);
}


static int lfo_setter_period(LFO *self, PyObject *val, void *closure) {
    self->period = PyFloat_AsDouble(val);

    return 0;
}

static PyObject * lfo_getter_cycles(LFO *self, void *closure) {
    return PyLong_FromLong(self->cycles);
}


static int lfo_setter_cycles(LFO *self, PyObject *val, void *closure) {
    self->cycles = PyLong_AsLong(val);

    return 0;
}


static PyObject * lfo_getter_frequency(LFO *self, void *closure) {
    return PyFloat_FromDouble(1.0 / self->period);
}


static int lfo_setter_frequency(LFO *self, PyObject *val, void *closure) {
    double frequency = PyFloat_AsDouble(val);

    self->period = 1.0 / frequency;

    return 0;
}

static PyObject * lfo_getter_frozen(LFO *self, void *closure) {
    if (self->frozen)
	Py_RETURN_TRUE;
    else
	Py_RETURN_FALSE;
}


static int lfo_setter_frozen(LFO *self, PyObject *val, void *closure) {
    if (PyObject_IsTrue(val)) {
	freeze(self);
    } else {
	unfreeze(self);
    }

    return 0;
}


static PyObject * lfo_getter_sine_attenuverter(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->sine_attenuverter);
}


static int lfo_setter_sine_attenuverter(LFO *self, PyObject *val, void *closure) {
    self->sine_attenuverter = PyFloat_AsDouble(val);

    return 0;
}


static PyObject * lfo_getter_cosine_attenuverter(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->cosine_attenuverter);
}


static int lfo_setter_cosine_attenuverter(LFO *self, PyObject *val, void *closure) {
    self->cosine_attenuverter = PyFloat_AsDouble(val);

    return 0;
}


static PyObject * lfo_getter_triangle_attenuverter(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->triangle_attenuverter);
}


static int lfo_setter_triangle_attenuverter(LFO *self, PyObject *val, void *closure) {
    self->triangle_attenuverter = PyFloat_AsDouble(val);

    return 0;
}


static PyObject * lfo_getter_sawtooth_attenuverter(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->sawtooth_attenuverter);
}


static int lfo_setter_sawtooth_attenuverter(LFO *self, PyObject *val, void *closure) {
    self->sawtooth_attenuverter = PyFloat_AsDouble(val);

    return 0;
}

static PyObject * lfo_getter_square_attenuverter(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->square_attenuverter);
}


static int lfo_setter_square_attenuverter(LFO *self, PyObject *val, void *closure) {
    self->square_attenuverter = PyFloat_AsDouble(val);

    return 0;
}

static PyObject * lfo_getter_one_attenuverter(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->one_attenuverter);
}


static int lfo_setter_one_attenuverter(LFO *self, PyObject *val, void *closure) {
    self->one_attenuverter = PyFloat_AsDouble(val);

    return 0;
}

static PyObject * lfo_getter_zero_attenuverter(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->zero_attenuverter);
}


static int lfo_setter_zero_attenuverter(LFO *self, PyObject *val, void *closure) {
    self->zero_attenuverter = PyFloat_AsDouble(val);

    return 0;
}


static PyObject * lfo_getter_sine_offset(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->sine_offset);
}


static int lfo_setter_sine_offset(LFO *self, PyObject *val, void *closure) {
    self->sine_offset = PyFloat_AsDouble(val);

    return 0;
}


static PyObject * lfo_getter_cosine_offset(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->cosine_offset);
}


static int lfo_setter_cosine_offset(LFO *self, PyObject *val, void *closure) {
    self->cosine_offset = PyFloat_AsDouble(val);

    return 0;
}


static PyObject * lfo_getter_triangle_offset(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->triangle_offset);
}


static int lfo_setter_triangle_offset(LFO *self, PyObject *val, void *closure) {
    self->triangle_offset = PyFloat_AsDouble(val);

    return 0;
}


static PyObject * lfo_getter_sawtooth_offset(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->sawtooth_offset);
}


static int lfo_setter_sawtooth_offset(LFO *self, PyObject *val, void *closure) {
    self->sawtooth_offset = PyFloat_AsDouble(val);

    return 0;
}


static PyObject * lfo_getter_square_offset(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->square_offset);
}

static int lfo_setter_square_offset(LFO *self, PyObject *val, void *closure) {
    self->square_offset = PyFloat_AsDouble(val);

    return 0;
}


static PyObject * lfo_getter_pw(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->pw);
}


static int lfo_setter_pw(LFO *self, PyObject *val, void *closure) {
    self->pw = PyFloat_AsDouble(val);

    return 0;
}


static PyObject * lfo_getter_pw_offset(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->pw_offset);
}


static int lfo_setter_pw_offset(LFO *self, PyObject *val, void *closure) {
    self->pw_offset = PyFloat_AsDouble(val);

    return 0;
}


static PyObject * lfo_getter_one_offset(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->one_offset);
}

static int lfo_setter_one_offset(LFO *self, PyObject *val, void *closure) {
    self->one_offset = PyFloat_AsDouble(val);

    return 0;
}


static PyObject * lfo_getter_zero_offset(LFO *self, void *closure) {
    return PyFloat_FromDouble(self->zero_offset);
}

static int lfo_setter_zero_offset(LFO *self, PyObject *val, void *closure) {
    self->zero_offset = PyFloat_AsDouble(val);

    return 0;
}


static PyObject * lfo_getter_t(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_t(self));
}


static PyObject * lfo_getter_normalized(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_normalized(self));
}


static PyObject * lfo_getter_cycle(LFO *self, void *closure) {
    return PyFloat_FromDouble(get_cycle(self));
}


/*----------------------------------------------------------------------
			 _       _      
     _ __ ___   ___   __| |_   _| | ___ 
    | '_ ` _ \ / _ \ / _` | | | | |/ _ \
    | | | | | | (_) | (_| | |_| | |  __/
    |_| |_| |_|\___/ \__,_|\__,_|_|\___|

----------------------------------------------------------------------*/

PyMODINIT_FUNC PyInit__lfo(void) {
    PyObject *m;

    if (PyType_Ready(&lfo_type) < 0)
	return NULL;

    m = PyModule_Create(&lfo_module);
    if (m == NULL)
	return NULL;

    Py_INCREF(&lfo_type);
    if (PyModule_AddObjectRef(m, "LFO", (PyObject *)&lfo_type) < 0) {
	Py_DECREF(&lfo_type);
	Py_DECREF(m);
	return NULL;
    }

    return m;
}
