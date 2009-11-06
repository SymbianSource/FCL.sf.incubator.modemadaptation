/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

NokiaTSY Test Tool -installer
-----------------------------

DESCRIPTION:
------------
This installer is made to guide you through the NokiaTSY Test Tool installation
process. It will check that you have the necessary components installed and it
will automatically modify the necessary files to run the tests. These
modifications are put behind flag, so you can easily turn them ON and OFF
after the installation.


INSTALLATION:
-------------
1. Check that you have your development environment is installed.
2. Check that you have mapped your environment correctly. (i.e. subst X: C:\Devlon\Devlon70wk47)
3. Check that you have the test tool in the CASW directory:
   X:\CASW_Integrated\casw\telephonyadaptation\nokiatsy\nokiatsy_dll\internal\test\nokiatsy_test_tool
4. Run nokiatsy_test_tool_installer.pl in folder (it is located there):
   X.\CASW_Integrated\casw\telephonyadaptation\nokiatsy\nokiatsy_dll\internal\test\nokiatsy_test_tool\doc

FUNCTIONALITY:
--------------
- This is what the installer does when run:

- Sets all files modifiable in nokiatsy folder and in it's sub folders.
- Checks that you have  the STIF framework - if not, exits .
- Checks that you have the NokiaTSY Test Tool - if not, exits .
- Searches certain directories for NokiaTSY installation
- Checks that you have the NokiaTSY Test Tool - if not, exits .

NOKIA TSY
- Runs reallyclean and recompiles NokiaTSY

NOKIA TSY TEST TOOL
 - Runs reallyclean and recompiles NokiaTSY Test Tool

NOKIA TSY
- Runs the rest of compilation (if test tool hasn't been compiled earlier the NTT_core.lib
  is missing and will appear after test tool compilation so re-compiling must be done).

- Executes the modifications to certain files. Here is the current modified file list:
	../NokiaTSY/group/NokiaTSY.mmp
	/epoc32/data/epoc.ini
	../IscIsiMultiplexer/group/IscIsiMultiplexer.mmp
	../STIFTestFramework/ConsoleUI/src/ConsoleUI.cpp
	/epoc32/winscw/c/TestFramework/TestFramework.ini

- Copies and Activates scripts found within test toool.

TURN ON/OFF:
------------
- To turn OFF the changes for NokiaTSY Test Tool, comment the following line in ..\NokiaTSY\group\NokiaTSY.mmp:
  #define INTERNAL_NTSY_TESTTOOL_MOD
  ...and run bldmake bldfiles + abld build
- To turn ON the changes for NokiaTSY Test Tool, make sure the following line in ..\NokiaTSY\group\NokiaTSY.mmp is NOT commented:
  #define INTERNAL_NTSY_TESTTOOL_MOD
  ...and run bldmake bldfiles + abld build

MODIFICATIONS:
--------------

It should be fairly easy to add basic modifications when needed. It is best to
add them to the end of the script for version control(it asks before
re-applying each change accoring to its position in the script file(ID)). In
the script there is a NEW MOD TASK TEMPLATE, which can be used when creating
new modifications. NOTE that the path to the file to be modified is in UNIX
format('/' instead of '\'). Below is a simple example, which you can modify
and add to the script after ADD MODIFICATIONS HERE:

##################### ADD NEW MODIFICATIONS HERE ######################

# THIS IS A TEST MODIFICATION
$mod_file{$mod_id} = "/epoc32/data/epoc.ini";
$mod_oper{$mod_id} = "addbelow"; #POSSIBLE KEYWORDS: ("afterany"|"replace"|"comment"|"addbelow")
$mod_srch{$mod_id} = "configuration epoc_176x208.ini";
$mod_text{$mod_id} = "#THIS WAS A TEST CASE FOR NOKIATSY TEST TOOL INSTALLER";
$mod_id++;

##################### END MODIFICATIONS HERE ######################
