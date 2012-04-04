/****************************************************************************/
/// @file    MSBitSetLogic.h
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id: MSBitSetLogic.h 8551 2010-04-02 14:42:58Z dkrajzew $
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
#ifndef MSBitSetLogic_h
#define MSBitSetLogic_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <bitset>
#include <vector>
#include <cassert>
#include "MSJunctionLogic.h"
#include "MSLogicJunction.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSBitSetLogic
 *
 * N is sum of the number of links of the junction's inLanes.
 */
template< size_t N >
class MSBitSetLogic : public MSJunctionLogic {
public:
    /** @brief Container that holds the right of way bitsets.
        Each link has it's own
        bitset. The bits in the bitsets correspond to the links. To create
        a bitset for a particular link, set the bits to true that correspond
        to links that have the right of way. All others set to false,
        including the link's "own" link-bit. */
    typedef std::vector< std::bitset< N > > Logic;

    /** @brief Container holding the information which internal lanes prohibt which links
        Build the same way as Logic */
    typedef std::vector< std::bitset< N > > Foes;


public:
    /// Use this constructor only.
    MSBitSetLogic(unsigned int nLinks,
                  unsigned int nInLanes,
                  Logic* logic,
                  Foes *foes,
                  std::bitset<64> conts)
            : MSJunctionLogic(nLinks, nInLanes), myLogic(logic),
            myInternalLinksFoes(foes), myConts(conts) {}


    /// Destructor.
    ~MSBitSetLogic() {
        delete myLogic;
        delete myInternalLinksFoes;
    }


    /// Modifies the passed respond according to the request.
    void respond(const MSLogicJunction::Request& request,
                 const MSLogicJunction::InnerState& innerState,
                 MSLogicJunction::Respond& respond) const {
        size_t i;
        // calculate respond
        for (i = 0; i < myNLinks; ++i) {
            if (myConts.test(i)) {
                respond.set(i, request.test(i));
            } else {
                bool linkPermit = request.test(i)
                                  &&
                                  ((request&(*myLogic)[i]).none() || myConts.test(i));
#ifdef HAVE_INTERNAL_LANES
                linkPermit &= (innerState&(*myInternalLinksFoes)[i]).none();
#endif
                respond.set(i, linkPermit);
            }
        }
    }


    /// Returns the foes of the given link
    const MSLogicJunction::LinkFoes &getFoesFor(unsigned int linkIndex) const throw() {
        return (*myLogic)[linkIndex];
    }

    const std::bitset<64> &getInternalFoesFor(unsigned int linkIndex) const throw() {
        return (*myInternalLinksFoes)[linkIndex];
    }

    bool getIsCont(unsigned int linkIndex) const throw() {
        return myConts.test(linkIndex);
    }

    virtual bool isCrossing() const throw() {
        for (typename Logic::const_iterator i=myLogic->begin(); i!=myLogic->end(); ++i) {
            if ((*i).any()) {
                return true;
            }
        }
        return false;
    }

private:
    /// junctions logic based on std::bitset
    Logic* myLogic;

    /// internal lanes logic
    Foes *myInternalLinksFoes;

    std::bitset<64> myConts;

private:
    /// @brief Invalidated copy constructor.
    MSBitSetLogic(const MSBitSetLogic&);

    /// @brief Invalidated assignment operator.
    MSBitSetLogic& operator=(const MSBitSetLogic&);

};


/** To make things easier we use a fixed size. 64 will be sufficient even for
    pathological junctions. If it will consume to much space, reduce it to 32.
    So, here comes the type which should be used by the netbuilder. */
typedef MSBitSetLogic< 64 > MSBitsetLogic;


#endif

/****************************************************************************/

