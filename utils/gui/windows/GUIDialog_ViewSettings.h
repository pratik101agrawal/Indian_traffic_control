/****************************************************************************/
/// @file    GUIDialog_ViewSettings.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 21. Dec 2005
/// @version $Id: GUIDialog_ViewSettings.h 8647 2010-04-26 12:24:55Z dkrajzew $
///
// The dialog to change the view (gui) settings.
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
#ifndef GUIDialog_ViewSettings_h
#define GUIDialog_ViewSettings_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/foxtools/MFXAddEditTypedTable.h>
#include <utils/foxtools/MFXMutex.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDialog_ViewSettings
 * @brief The dialog to change the view (gui) settings.
 *
 * @todo Check whether saving/loading settings should be done via XML
 */
class GUIDialog_ViewSettings : public FXDialogBox {
    // is a FOX-object with an own mapping
    FXDECLARE(GUIDialog_ViewSettings)
public:
    /** @brief Constructor
     * @param[in] parent The view to report changed settings to
     * @param[in, out] settings The current settings that can be changed
     * @param[in] laneEdgeModeSource Class storing known lane coloring schemes
     * @param[in] vehicleModeSource Class storing known vehicle coloring schemes
     * @param[in] decals Decals used
     * @param[in] decalsLock A lock to set if the decals are changed
     */
    GUIDialog_ViewSettings(GUISUMOAbstractView *parent,
                           GUIVisualizationSettings *settings,
                           std::vector<GUISUMOAbstractView::Decal> *decals,
                           MFXMutex *decalsLock) throw();


    /// @brief Destructor
    ~GUIDialog_ViewSettings() throw();


    /** @brief Sets current settings (called if reopened)
     * @param[in, out] settings The current settings that can be changed
     */
    void setCurrent(GUIVisualizationSettings *settings) throw();



    /// @name FOX-callbacks
    /// @{

    /// @brief Called if the OK-button was pressed
    long onCmdOk(FXObject*,FXSelector,void*);

    /// @brief Called if the Cancel-button was pressed
    long onCmdCancel(FXObject*,FXSelector,void*);

    /// @brief Called if something (color, width, etc.) has been changed
    long onCmdColorChange(FXObject*,FXSelector,void*);

    /// @brief Called if the decals-table was changed
    long onCmdEditTable(FXObject*,FXSelector,void*data);

    /// @brief Called if the name of the scheme was changed
    long onCmdNameChange(FXObject*,FXSelector,void*);

    /// @brief Called if the settings shall be saved into the registry
    long onCmdSaveSetting(FXObject*,FXSelector,void*data);
    /// @brief Called when updating the button that allows to save the settings into the registry
    long onUpdSaveSetting(FXObject*,FXSelector,void*data);

    /// @brief Called if the settings shall be deleted
    long onCmdDeleteSetting(FXObject*,FXSelector,void*data);
    /// @brief Called when updating the button that allows to delete settings
    long onUpdDeleteSetting(FXObject*,FXSelector,void*data);

    /// @brief Called if the settings shall be exported into a file
    long onCmdExportSetting(FXObject*,FXSelector,void*data);
    /// @brief Called when updating the button that allows to export settings into a file
    long onUpdExportSetting(FXObject*,FXSelector,void*data);

    /// @brief Called if the settings shall be read from a file
    long onCmdImportSetting(FXObject*,FXSelector,void*data);
    /// @brief Called when updating the button that allows to read settings from a file
    long onUpdImportSetting(FXObject*,FXSelector,void*data);

    /// @brief Called if the decals shall be loaded from a file
    long onCmdLoadDecals(FXObject*,FXSelector,void*data);
    /// @brief Called if the decals shall be saved to a file
    long onCmdSaveDecals(FXObject*,FXSelector,void*data);
    /// @}



    /** @brief Returns the name of the currently chosen scheme
     * @return The name of the edited (chosen) scheme)
     */
    std::string getCurrentScheme() const throw();


    /** @brief Sets the named scheme as the current
     * @param[in] The name of the scheme that shall be set as current
     */
    void setCurrentScheme(const std::string &) throw();


protected:
    /** @brief Converts a FOX-color into a RGBColor
     * @param[in] c The color to convert
     * @return The converted color
     */
    RGBColor convert(const FXColor c) const throw();


    /** @brief Converts a RGBColor into a FOX-color
     * @param[in] c The color to convert
     * @return The converted color
     */
    FXColor convert(const RGBColor &c) const throw();


    /** @brief Rebuilds color changing dialogs after choosing another coloring scheme
     * @param[in] doCreate Whether "create" shall be called (only if built the first time)
     */
    void rebuildColorMatrices(bool doCreate=false) throw();


    /** @brief Rebuilds the decals table */
    void rebuildList() throw();


    /** @brief Loads a scheme from a file
     * @param[in] file The name of the file to read the settings from
     */
    void loadSettings(const std::string &file) throw();


    /** @brief Writes the currently used decals into a file
     * @param[in] file The name of the file to write the decals into
     */
    void saveDecals(const std::string &file) const throw();


    /** @brief Loads decals from a file
     * @param[in] file The name of the file to read the decals from
     */
    void loadDecals(const std::string &file) throw();


private:
    /// @brief The parent view (which settings are changed)
    GUISUMOAbstractView *myParent;

    /// @brief The current settings
    GUIVisualizationSettings *mySettings;

    /// @brief A backup of the settings (used if the "Cancel" button is pressed)
    GUIVisualizationSettings myBackup;

    /// @brief The parent's decals
    std::vector<GUISUMOAbstractView::Decal> *myDecals;

    /// @brief Lock used when changing the decals
    MFXMutex *myDecalsLock;


    /// @name Dialog elements
    /// @{
    FXComboBox *mySchemeName;
    FXCheckButton *myShowGrid;
    FXRealSpinDial *myGridXSizeDialer, *myGridYSizeDialer;

    FXColorWell *myBackgroundColor;
    FXVerticalFrame *myDecalsFrame;
    MFXAddEditTypedTable *myDecalsTable;

    FXComboBox *myLaneEdgeColorMode;
    FXVerticalFrame *myLaneColorSettingFrame;
    std::vector<FXColorWell *> myLaneColors;
    std::vector<FXRealSpinDial *> myLaneThresholds;
    std::vector<FXButton *> myLaneButtons;
    FXCheckButton *myLaneColorInterpolation;

    FXCheckButton *myShowLaneBorders, *myShowLaneDecals, *myShowRails, *myShowEdgeName, *myShowInternalEdgeName, *myHideMacroConnectors;
    FXRealSpinDial *myEdgeNameSizeDialer;
    FXColorWell *myEdgeNameColor;
    FXRealSpinDial *myInternalEdgeNameSizeDialer;
    FXColorWell *myInternalEdgeNameColor;

    FXComboBox *myVehicleColorMode, *myVehicleShapeDetail;
    FXVerticalFrame *myVehicleColorSettingFrame;
    std::vector<FXColorWell *> myVehicleColors;
    std::vector<FXRealSpinDial *> myVehicleThresholds;
    std::vector<FXButton *> myVehicleButtons;
    FXCheckButton *myVehicleColorInterpolation;
    FXRealSpinDial *myVehicleMinSizeDialer, *myVehicleUpscaleDialer;
    FXCheckButton *myShowBlinker, /* *myShowLaneChangePreference,*/ *myShowVehicleName;
    FXRealSpinDial *myVehicleNameSizeDialer;
    FXColorWell *myVehicleNameColor;

    FXCheckButton *myShowTLIndex, *myShowJunctionIndex;
    FXCheckButton *myShowJunctionName;
    FXRealSpinDial *myJunctionNameSizeDialer;
    FXColorWell *myJunctionNameColor;

    FXRealSpinDial *myDetectorMinSizeDialer, *myDetectorUpscaleDialer, *myAddNameSizeDialer;
    FXCheckButton *myShowAddName;

    FXRealSpinDial *myPOIMinSizeDialer, *myPOIUpscaleDialer, *myPOINameSizeDialer;
    FXCheckButton *myShowPOIName;
    FXColorWell *myPOINameColor;

    FXCheckButton *myShowLane2Lane;
    FXCheckButton *myAntialiase;
    FXCheckButton *myDither;
    FXCheckButton *myShowSizeLegend;
    /// @}


protected:
    /// @brief Default constructor (needed by FOX)
    GUIDialog_ViewSettings() { }


private:
    /// @brief invalidated copy constructor
    GUIDialog_ViewSettings(const GUIDialog_ViewSettings &s);

    /// @brief invalidated assignment operator
    GUIDialog_ViewSettings &operator=(const GUIDialog_ViewSettings &s);


};


#endif

/****************************************************************************/

