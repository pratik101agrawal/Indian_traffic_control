/****************************************************************************/
/// @file    CastingFunctionBinding.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id: CastingFunctionBinding.h 8236 2010-02-10 11:16:41Z behrisch $
///
//	�missingDescription�
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef CastingFunctionBinding_h
#define CastingFunctionBinding_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/ValueSource.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CastingFunctionBinding
 */
template< class T, typename R, typename O  >
class CastingFunctionBinding : public ValueSource<R> {
public:
    /// Type of the function to execute.
    typedef O(T::* Operation)() const;

    CastingFunctionBinding(T* source, Operation operation) :
            mySource(source),
            myOperation(operation) {}

    /// Destructor.
    ~CastingFunctionBinding() {}

    R getValue() const {
        return (R)(mySource->*myOperation)();
    }

    ValueSource<R> *copy() const {
        return new CastingFunctionBinding<T, R, O>(mySource, myOperation);
    }

    ValueSource<SUMOReal> *makeSUMORealReturningCopy() const {
        return new CastingFunctionBinding<T, SUMOReal, O>(mySource, myOperation);
    }

protected:

private:
    /// The object the action is directed to.
    T* mySource;

    /// The object's operation to perform.
    Operation myOperation;
};


#endif

/****************************************************************************/

