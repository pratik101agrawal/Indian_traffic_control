/****************************************************************************/
/// @file    MFXUtils.h
/// @author  Daniel Krajzewicz
/// @date    2006-01-09
/// @version $Id: MFXUtils.h 8236 2010-02-10 11:16:41Z behrisch $
///
// Some helper functions for FOX
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
#ifndef MFXUtils_h
#define MFXUtils_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MFXUtils
 * @brief Some helper functions for FOX
 */
class MFXUtils {
public:
    /** @brief Deletes all children of the given window
     *
     * @param[in] w The window to delete all of his children
     */
    static void deleteChildren(FXWindow *w) throw();


    /** @brief Returns true if either the file given by its name does not exist or the user allows overwriting it
     *
     * If the named file does not exist, true is returned. Otherwise, a message
     *  box is prompted that asks whether the file may be replaced. If the user
     *  answers "yes" in this case, true is returned. In any other cases
     *  ("no"/"cancel"), false.
     *
     * @param[in] parent A parent window needed to prompt the dialog box
     * @param[in] file The file to check whether it may be generated
     * @return Whether the named file may be written
     */
    static FXbool userPermitsOverwritingWhenFileExists(
        FXWindow * const parent, const FXString &file) throw();


    /** @brief Returns the title text in dependance to an optional file name
     *
     * The title is computed as default on windows: The application name only if no
     *  file name is given. If a file name is given, it is used without the extension,
     *  extended by the application name.
     *
     * @param[in] appname The name of the application to return the title of
     * @param[in] appname The name of the file loaded by the application
     * @return The built title
     */
    static FXString getTitleText(const FXString &appname,
                                 FXString filename="") throw();


    /** @brief Returns the document name
     *
     * Removes the path first. Then, returns the part before the first '.'
     *  occurence of the so obtained string.
     *
     * @param[in] filename The file name (including the path) to obtain the name of
     * @return The name (without the path and the extension)
     */
    static FXString getDocumentName(const FXString &filename) throw();


    /** @brief Corrects missing extension
     *
     * At first, the extension is determined. If there is none, the given default
     *  extension is appended to the file name/path. Otherwise the
     *  file name/path remains as is.
     * The so obtained correct file name is returned.
     * @param[in] filename The filename to evaluate
     * @param[in] defaultExtension The default extension to use
     * @return The corrected filename (with extension if no one was given
     */
    static FXString assureExtension(const FXString &filename, const FXString &defaultExtension) throw();


    /** @brief Returns the file name to write
     *
     * A somehow complete procedure for determining the file name of a file
     *  to write. Builds a file dialog, checks whether a file was chosen,
     *  if so, checks whether it's not existing or the user allows to
     *  overwrite it etc.
     *
     * Returns an empty string if the file shall not be created, the
     *  filename if it shall.
     *
     * @param[in] parent The window needed to display dialogs
     * @param[in] header Title of the save-dialog
     * @param[in] extension The extension the file should have (must be in the form '.xxx'
     * @param[in] icon The icon the dialog should have
     * @param[in] currentFolder The string into which the information about the current folder shall be saved
     * @return The name of the file to write
     */
    static FXString getFilename2Write(FXWindow *parent,
                                      const FXString &header, const FXString &extension,
                                      FXIcon *icon, FXString &currentFolder) throw();

};


#endif

/****************************************************************************/

