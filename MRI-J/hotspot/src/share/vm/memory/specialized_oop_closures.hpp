/*
 * Copyright 2001-2006 Sun Microsystems, Inc.  All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
 *  
 */
// This file is a derivative work resulting from (and including) modifications
// made by Azul Systems, Inc.  The date of such changes is 2010.
// Copyright 2010 Azul Systems, Inc.  All Rights Reserved.
//
// Please contact Azul Systems, Inc., 1600 Plymouth Street, Mountain View, 
// CA 94043 USA, or visit www.azulsystems.com if you need additional information 
// or have any questions.
#ifndef SPECIALIZED_OOP_CLOSURES_HPP
#define SPECIALIZED_OOP_CLOSURES_HPP


// The following OopClosure types get specialized versions of
// "oop_oop_iterate" that invoke the closures' do_oop methods
// non-virtually, using a mechanism defined in this file.  Extend these
// macros in the obvious way to add specializations for new closures.

// Forward declarations.
class OopClosure;
class OopsInGenClosure;
// DefNew
class ScanClosure;
class FastScanClosure;
class FilteringClosure;
class LVB_CloneClosure;


// This macro applies an argument macro to all OopClosures for which we
// want specialized bodies of "oop_oop_iterate".  The arguments to "f" are:
//   "f(closureType, non_virtual)"
// where "closureType" is the name of the particular subclass of OopClosure,
// and "non_virtual" will be the string "_nv" if the closure type should
// have its "do_oop" method invoked non-virtually, or else the
// string "_v".  ("OopClosure" itself will be the only class in the latter
// category.)

// This is split into several because of a Visual C++ 6.0 compiler bug
// where very long macros cause the compiler to crash

#define SPECIALIZED_OOP_OOP_ITERATE_CLOSURES_S(f)       \
  f(ScanClosure,_nv)                                    \
  f(FastScanClosure,_nv)                                \
f(FilteringClosure,_nv)\
f(LVB_CloneClosure,_nv)

#define SPECIALIZED_OOP_OOP_ITERATE_CLOSURES_P(f)

#define SPECIALIZED_OOP_OOP_ITERATE_CLOSURES_1(f)       \
  SPECIALIZED_OOP_OOP_ITERATE_CLOSURES_S(f)             \
  SPECIALIZED_OOP_OOP_ITERATE_CLOSURES_P(f)
  
#define SPECIALIZED_OOP_OOP_ITERATE_CLOSURES_2(f)

#define SPECIALIZED_OOP_OOP_ITERATE_CLOSURES_3(f)

// We separate these out, because sometime the general one has
// a different definition from the specialized ones, and sometimes it
// doesn't.

#define ALL_OOP_OOP_ITERATE_CLOSURES_1(f)               \
  f(OopClosure,_v)                                      \
  SPECIALIZED_OOP_OOP_ITERATE_CLOSURES_1(f)

#define ALL_OOP_OOP_ITERATE_CLOSURES_3(f)               \
  SPECIALIZED_OOP_OOP_ITERATE_CLOSURES_3(f)

// This macro applies an argument macro to all OopClosures for which we
// want specialized bodies of a family of methods related to
// "par_oop_iterate".  The arguments to f are the same as above.
// The "root_class" is the most general class to define; this may be
// "OopClosure" in some applications and "OopsInGenClosure" in others.
#define SPECIALIZED_PAR_OOP_ITERATE_CLOSURES(f)

#define ALL_PAR_OOP_ITERATE_CLOSURES(f)                \
  f(OopClosure,_v)                                     \
  SPECIALIZED_PAR_OOP_ITERATE_CLOSURES(f)

// This macro applies an argument macro to all OopClosures for which we
// want specialized bodies of a family of methods related to
// "oops_since_save_marks_do".  The arguments to f are the same as above.
// The "root_class" is the most general class to define; this may be
// "OopClosure" in some applications and "OopsInGenClosure" in others.

#define SPECIALIZED_SINCE_SAVE_MARKS_CLOSURES_YOUNG_S(f) \
  f(ScanClosure,_nv)					 \
  f(FastScanClosure,_nv)

#define SPECIALIZED_SINCE_SAVE_MARKS_CLOSURES_YOUNG_P(f)

#define SPECIALIZED_SINCE_SAVE_MARKS_CLOSURES_YOUNG(f)  \
  SPECIALIZED_SINCE_SAVE_MARKS_CLOSURES_YOUNG_S(f)      \
  SPECIALIZED_SINCE_SAVE_MARKS_CLOSURES_YOUNG_P(f)

#define SPECIALIZED_SINCE_SAVE_MARKS_CLOSURES(f)        \
  SPECIALIZED_SINCE_SAVE_MARKS_CLOSURES_YOUNG(f)

// We separate these out, because sometime the general one has
// a different definition from the specialized ones, and sometimes it
// doesn't.
// NOTE:   One of the valid criticisms of this
// specialize-oop_oop_iterate-for-specific-closures idiom is that it is
// easy to have a silent performance bug: if you fail to de-virtualize,
// things still work, just slower.  The "SpecializationStats" mode is
// intended to at least make such a failure easy to detect.
// *Not* using the ALL_SINCE_SAVE_MARKS_CLOSURES(f) macro defined
// below means that *only* closures for which oop_oop_iterate specializations
// exist above may be applied to "oops_since_save_marks".  That is,
// this form of the performance bug is caught statically.  When you add
// a definition for the general type, this property goes away.
// Make sure you test with SpecializationStats to find such bugs
// when introducing a new closure where you don't want virtual dispatch.

#define ALL_SINCE_SAVE_MARKS_CLOSURES(f)                \
  f(OopsInGenClosure,_v)                                \
  SPECIALIZED_SINCE_SAVE_MARKS_CLOSURES(f)

// For keeping stats on effectiveness.
#define ENABLE_SPECIALIZATION_STATS 0


class SpecializationStats {
public:
  enum Kind {
    ik,             // instanceKlass
    irk,            // instanceRefKlass
    oa,             // objArrayKlass
    NUM_Kinds
  };

#if ENABLE_SPECIALIZATION_STATS
private:
  static int _numCallsAll;

  static int _numCallsTotal[NUM_Kinds];
  static int _numCalls_nv[NUM_Kinds];

  static int _numDoOopCallsTotal[NUM_Kinds];
  static int _numDoOopCalls_nv[NUM_Kinds];
public:
#endif
  static void clear()  PRODUCT_RETURN;

  static inline void record_call()  PRODUCT_RETURN;
  static inline void record_iterate_call_v(Kind k)  PRODUCT_RETURN;
  static inline void record_iterate_call_nv(Kind k)  PRODUCT_RETURN;
  static inline void record_do_oop_call_v(Kind k)  PRODUCT_RETURN;
  static inline void record_do_oop_call_nv(Kind k)  PRODUCT_RETURN;

  static void print() PRODUCT_RETURN;
};  

#ifndef PRODUCT
#if ENABLE_SPECIALIZATION_STATS

inline void SpecializationStats::record_call() {
  _numCallsAll++;;
}
inline void SpecializationStats::record_iterate_call_v(Kind k) {
  _numCallsTotal[k]++;
}
inline void SpecializationStats::record_iterate_call_nv(Kind k) {
  _numCallsTotal[k]++;
  _numCalls_nv[k]++;
}

inline void SpecializationStats::record_do_oop_call_v(Kind k) {
  _numDoOopCallsTotal[k]++;
}
inline void SpecializationStats::record_do_oop_call_nv(Kind k) {
  _numDoOopCallsTotal[k]++;
  _numDoOopCalls_nv[k]++;
}

#else   // !ENABLE_SPECIALIZATION_STATS

inline void SpecializationStats::record_call() {}
inline void SpecializationStats::record_iterate_call_v(Kind k) {}
inline void SpecializationStats::record_iterate_call_nv(Kind k) {}
inline void SpecializationStats::record_do_oop_call_v(Kind k) {}
inline void SpecializationStats::record_do_oop_call_nv(Kind k) {}
inline void SpecializationStats::clear() {}
inline void SpecializationStats::print() {}

#endif  // ENABLE_SPECIALIZATION_STATS
#endif  // !PRODUCT
#endif  // SPECIALIZED_OOP_CLOSURES_HPP
