#
# Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#

########################################## VARIABLES ##########################################

$appname_and_version  = "NokiaTSY Test Tool - installer v1";
$date_and_author    = "20-Nov-2008 Martti Suvanne";

#info string
$syntax =           "\n----------------------------------------------------------------------------\n";
$syntax = $syntax.  " $appname_and_version  $date_and_author\n";
$syntax = $syntax.  "----------------------------------------------------------------------------\n\n";
$syntax = $syntax.  "Checks that the environment is ready for testing, and makes the necessary\n";
$syntax = $syntax.  "changes to NokiaTSY. \n";
$syntax = $syntax.  "NOTE: you have to have your environment mapped as a virtual drive \n\n\n";
$syntax = $syntax.  "";
$syntax = $syntax.  "*************************************************************************";
$syntax = $syntax.  "*** THIS SCRIPT SHOULD BE RUN IN THE DIRECTORY IN WHICH IT IS LOCATED ***";
$syntax = $syntax.  "*************************************************************************";
$syntax = $syntax.  "";
$syntax = $syntax.  "Usage: install.pl [/parameters] \n\n";
$syntax = $syntax.  "       /?            Show this help.\n";
$syntax = $syntax.  "       /N            NO to ALL, this installs ONLY the modifications that are\n";
$syntax = $syntax.  "                      not overwriting anything. And does NOT compile anything.\n";
$syntax = $syntax.  "       /Y            YES to ALL, this installs ALL the modifications and\n";
$syntax = $syntax.  "                      compiles everything.\n";
$syntax = $syntax.  "       /cs           Activate scripts only.\n";
$syntax = $syntax.  "       /compileonly  YES to all COMPILATIONS.\n";
$syntax = $syntax.  "       /modifyonly   YES to all COMPILATIONS.\n";
$syntax = $syntax.  "       /version  show version and environment information\n\n\n";
$syntax = $syntax.  "HELP END.\n----------------------------------------------------------------------------\n";

#GLOBAL VARs
$stif_installation  = 0;  #0-no stiff, 1-stiff exist, but not compiled, 2-stif ok
$testtool_installation  = 0;  #0-no ntsytt, 1-ntsytt exist, but not compiled, 2-ntsytt ok
$nokiatsy_installation  = 0;  #0-no ntsy, 1-ntsy exist, but not compiled, 2-ntsy ok

$compileall   = 0; #ask, yes, no
$modifyall  = 0; #ask, yes, no

$mod_flag   = "INTERNAL_NTSY_TESTTOOL_MOD";
$nokiatsy_path  = "";

########################################## SCRIPT STARTS ##########################################

$amount = @ARGV;
#if($amount==0){die "\nHelp:".$syntax;} #empty

# Make files modifiable
chdir ("cd ../../../../../..");
system ("attrib -R /S");

for($i_args=0;$i_args<$amount;$i_args++)
  {
  #HELP
  if   (@ARGV[$i_args] =~ m/^[\/-](\?|h|help)$/i)
    {
    die "\nHelp:".$syntax;
    }
  #VERSION
  elsif(@ARGV[$i_args] =~ m/^[\/-](v|version)$/i)
    {
    die "\n$appname_and_version - $date_and_author\n\n";
    }
  #N
  elsif(@ARGV[$i_args] =~ m/^[\/-](n|no)$/i)
    {
    print "\nNO to all.\n";
    $compileall =2;
    $modifyall  =2;
    }
  #Y
  elsif(@ARGV[$i_args] =~ m/^[\/-](y|yes)$/i)
    {
    print "\nYES to all.\n";
    $compileall =1;
    $modifyall  =1;
    }
  #compileonly
  elsif(@ARGV[$i_args] =~ m/^[\/-](compileonly)$/i)
    {
    print "\nCOMPILE only.\n";
    $compileall =1;
    $modifyall  =2;
    }
  #modifyonly
  elsif(@ARGV[$i_args] =~ m/^[\/-](modifyonly)$/i)
    {
    print "\nMODIFY only.\n";
    $compileall =2;
    $modifyall  =1;
    }
  #modifyonly
  elsif(@ARGV[$i_args] =~ m/^[\/-](cs|copyscript)$/i)
    {
    print "\nSCRIPT COPY only.\n";
    $compileall =2;
    $modifyall  =2;
    $testroot = "";
    $testroot = ( !length($testroot) && (-e "/CASW_Integrated/modemadaptation/adaptationlayer/tsy/nokiatsy_dll/internal/test/nokiatsy_test_tool/group/bld.inf"))? "/casw" : $testroot; #CASW
    $testroot = ( !length($testroot) && (-e "/nokiatsy_test_tool/core/group/bld.inf"))? "/" : $testroot; #root
    if(!length($testroot))
      {
      die "NO NTSY Test Tool installation found.\n";
      }
    goto ACTIVATESCRIPTS;
    }
  #BAD INPUT
  else
    {
    die "Invalid parameter($i_args): @ARGV[$i_args].\n";
    }
  }

print "\n----------------------------------------------------------------------------\n $appname_and_version  $date_and_author\n----------------------------------------------------------------------------\n";


### FIND PATHS ###

$stifroot = "";
$stifroot = ( !length($stifroot) && (-e "/CASW_Integrated/stif/TestEngine/group/bld.inf"))? "/CASW_Integrated" : $stifroot;          #CASW
$stifroot = ( !length($stifroot) && (-e "/stif/TestEngine/group/bld.inf"))? "/" : $stifroot;   #root
if( !length($stifroot) )
  {
  die "NO STIF installation found.\n";
  }

print "   - PATH: \"".$stifroot."\"\n";

$testroot = "";
$testroot = ( !length($testroot) && (-e "/CASW_Integrated/modemadaptation/adaptationlayer/tsy/nokiatsy_dll/internal/test/nokiatsy_test_tool/group/bld.inf"))? "/CASW_Integrated/modemadaptation/adaptationlayer/tsy/nokiatsy_dll/internal/test" : $testroot; #CASW
$testroot = ( !length($testroot) && (-e "/nokiatsy_test_tool/core/group/bld.inf"))? " " : $testroot; #root
if(!length($testroot))
  {
  die "NO NTSY Test Tool installation found.\n";
  }
print "   - PATH: \"".$testroot."\"\n";

$nokiatsy_path = "";
$nokiatsy_path = ( !length($nokiatsy_path) && (-e "/CASW_Integrated/modemadaptation/adaptationlayer/tsy/group/bld.inf"))? "/CASW_Integrated/modemadaptation/adaptationlayer" : $nokiatsy_path;                   #CASW
#$nokiatsy_path = ( !length($nokiatsy_path) && (-e "/ncp_sw/corecom/modemadaptation/adaptationlayer/tsy/group/bld.inf"))? "/ncp_sw/corecom/modemadaptation/adaptationlayer" : $nokiatsy_path;   #3.0 RND
#$nokiatsy_path = ( !length($nokiatsy_path) && (-e "/ape_cmt/NokiaTSY/NokiaTSY/group/bld.inf"))? "/ape_cmt/NokiaTSY" : $nokiatsy_path;                                   #2.8 RND
#$nokiatsy_path = ( !length($nokiatsy_path) && (-e "/bs_adaptation/casw_adaptation/NokiaTSY/NokiaTSY/group/bld.inf" ))? "/bs_adaptation/casw_adaptation/NokiaTSY" : $nokiatsy_path;            #OLD 3.0 RND
#$nokiatsy_path = ( !length($nokiatsy_path) && (-e "/baseservices/adaptation/nokiaadaptation/NokiaTSY/NokiaTSY/group/bld.inf"))? "/baseservices/adaptation/nokiaadaptation/NokiaTSY" : $nokiatsy_path;   #2.6
if(!length($nokiatsy_path))
  {
  die "NO NokiaTSY installation found.\n";
  }
print "   - PATH: \"".$nokiatsy_path."\"\n";


########################################## MODIFICATIONs START ##########################################
#
# Do the modifications to different files

print "\n____________________________________________________________________________\n1. INSTALLING CHANGES.\n";

if(2!=$modifyall) #NO MODIFY
  {
  $mod_amount = &AddMods();
  #print "MODS: $mod_amount";

  for($i_mods=0;$i_mods<$mod_amount;$i_mods++)
    {
    &Modify($mod_file{$i_mods}, $mod_oper{$i_mods}, $mod_srch{$i_mods}, $mod_text{$i_mods},$i_mods, $mod_nodf{$i_mods});
    }

  }
else
  {
  print "   - MODIFICATIONS SKIPPED";
  }

########################################## COMPILING PHASES ##########################################
#
# it is checked that all the components required are installed properly
# also if some components haven't been compiled, user is prompted for compilation
#

#--------------------------------------------------------------------------
# NokiaTSY
#--------------------------------------------------------------------------
print "\n____________________________________________________________________________\n4. NOKIATSY.\n";
$nokiatsy_installation |= (-e $nokiatsy_path."/tsy/group/bld.inf")? 1 : 0;
$nokiatsy_installation |= (-e "/epoc32/release/winscw/UREL/NOKIATSY.DLL")? 2 : 0;


print "   - Clean and export for NokiaTSY (export is needed for Nokia TSY Test Tool)";

#INSTALLATION DIR seems to exist. Clean and export (export is needed for Nokia TSY Test Tool).
if( $nokiatsy_installation&1 )
  {
  #COMPILE
  print "   - CLEAN AND EXPORT. \n";

  $cpath= $nokiatsy_path."/tsy/group";
  &Cleanandcompile($cpath,1);
  }
else
  {
  print "   - NokiaTSY was not found! You need to extract the NokiaTSY in order to use the tool\n     You can for example get a snapshot\n     from NokiaTSY's newest release project from Synergy.\n";
  print " ...ERROR.\n";
  exit;
  }
print " ...OK.\n";


#--------------------------------------------------------------------------
# NTSY Test Tool
#--------------------------------------------------------------------------
print "\n____________________________________________________________________________\n3. NOKIATSY TEST TOOL.\n";
$testtool_installation |= (-e $testroot."/nokiatsy_test_tool/core/group/bld.inf")? 1 : 0;
$testtool_installation |= (-e "/epoc32/release/winscw/UREL/NTT_core.dll")? 2 : 0;


#INSTALLATION DIR seems to exist
if( $testtool_installation&1 )
  {
  $cpath=$testroot."/NokiaTSY_Test_Tool/group";
  &Compile($cpath);
  }
else #if($stif_installation==0 || $stif_installation==2)
  {
  print "   - Test Tool was not found! You need to extract the NokiaTSY Test Tool in order to use it\n     You can for example get a snapshot\n     from NokiaTSY_Test_Tool's newest release project from Synergy.\n";
  print " ...ERROR.\n";
  exit;
  }
print " ...OK.\n";





#--------------------------------------------------------------------------
# NokiaTSY
#--------------------------------------------------------------------------
print "\n____________________________________________________________________________\n4. NOKIATSY.\n";
$nokiatsy_installation |= (-e $nokiatsy_path."/tsy/group/bld.inf")? 1 : 0;
$nokiatsy_installation |= (-e "/epoc32/release/winscw/UREL/NOKIATSY.DLL")? 2 : 0;


#INSTALLATION DIR seems to exist
if( $nokiatsy_installation&1 )
  {
  #COMPILE
  $cpath= $nokiatsy_path."/tsy/group";
  &Compile($cpath,1);
  }
else
  {
  print "   - NokiaTSY was not found! You need to extract the NokiaTSY in order to use the tool\n     You can for example get a snapshot\n     from NokiaTSY's newest release project from Synergy.\n";
  print " ...ERROR.\n";
  exit;
  }
print " ...OK.\n";


########################################## ACTIVATE ALL SCRIPTS ##########################################
ACTIVATESCRIPTS:
print "\n____________________________________________________________________________\n6. COPY SCRIPTs.\n";
if(1!=$compileall)
  {
  #ACTIVATE
  @filesfrom;
  use File::Find;
  find(\&SearchScripts, $testroot.'/NokiaTSY_Test_Tool'); #Fills the $filesfrom
  @filesto2 = ('/epoc32/winscw/c/TestFramework/TestFramework.ini');
  print "\n  - ACTIVATE SCRIPTS:\n     (NOTE: If you activate ALL the scripts, it may slow down starting test)\n";
  $fop = &ActivateScripts( \@filesfrom, \@filesto2 ); #passing references
  if( $fop != 1)
    {
    print "   - error is script activation: $fop\n";
    }
  else
    {
    print "   - activating ok.\n";
    }
  }



########################################## ENDING ##########################################

#ENDING
print "\n____________________________________________________________________________\n\n";
print "   - Installer ended, to start the NokiaTSY Test Tool:\n     Start emulator and give a command in prompt: consoleui\n     and browse test cases using arrow keys.\n\n";
exit;

########################################## MAIN END ##########################################
#----------------------------------------- MAIN END -----------------------------------------#











########################################## SUBROUTINES ##########################################

######### Compile and clean
sub Compile
  {
  local($comppath)    = $_[0]; #Compilation path
  local($notreally)     = $_[1]; #No cleaning wanted

  chdir $comppath;
  print "   - PREPARE BUILD FILES. \n";
  system("bldmake bldfiles") == 0 or exit;

  if(!$notreally)
    {
    print "   - CLEAN OLD. \n";
    system("abld reallyclean winscw") == 0 or exit;
    }
  print "   - BUILD WINSCW. \n";
  system("abld build winscw") == 0 or exit;
  }

######### Compile and clean
sub Cleanandcompile
  {
  local($comppath)    = $_[0]; #Compilation path

  chdir $comppath;
  print "   - PREPARE BUILD FILES. \n";
  system("bldmake bldfiles") == 0 or exit;

  print "   - CLEAN OLD. \n";
  system("abld reallyclean winscw nokiatsy_dll") == 0 or exit;

  print "   - BUILD WINSCW. \n";
  system("abld build winscw nokiatsy_dll") == 0 or exit;

  }

######### Search
sub SearchScripts
  {
  local($filetype) = 'cfg';
  if($_ =~ m/\.$filetype$/i)
    {
    print "      ".$File::Find::name."\n";
    push(@filesfrom,$File::Find::name)
    }
  }

######### Modify files
sub Modify
  {
  local($filepath)  = $_[0];
  local($operation)   = $_[1];
  local($search)    = $_[2];
  local($replace)   = $_[3];
  local($id)    = $_[4];
  local($nodef)     = $_[5];

  local($mod_def_start)   ="";
  local($mod_def_else)  ="";
  local($mod_def_end)   ="";
  local($data) = "";
  local($permission) = 1;

  #make paths to correct format
  $filepath =~ s/\\/\//g;

  my($directory, $filename) = $filepath =~ m/(.*\/)(.*)$/;
  local($linecomment) = ($filename =~ /\.ini$/i )?"#":"//";

  #flagging
  local($ifdefs) = ($filename =~ /\.(cpp|h|mmp)$/i )?1:0;
  local($mod_def_name) = ($filename =~ /\.mmp$/i )?$mod_flag."_MMP":$mod_flag;
  #print "def: $mod_def_name\n";

  local($changenote)  = "\n\n".$linecomment."NOKIATSY TEST TOOL (ID:".$id.")";

  #OPEN FILE
  print "\n   - modifying file: $filename\n    -- reading..\n";
  if(!open( RESOURCE, "< $filepath" ))
    {
    print "    -- NOT FOUND! skipped\n";
    return;
    }

  #read file
  while ( <RESOURCE> )
    {
    $data .= $_;
    }
  close( RESOURCE  );

  #check if already modified
  $cn=$changenote;
  $cn =~ s/\s+/\\s\*/g;
  $cn =~ s/([().+?\/])/\\$1/g;
  if($data =~ /$cn/ )
    {
    print "    -- File has been modified already, proceed anyway? [y/n] ";

    $modcomp= &ReadKey();

    if($modcomp =~ m/^[^y]/i)
      {
      $permission = 0;
      }
    }

  if($permission!=0)
    {
    print "    -- applying modification\n";

    if($ifdefs!=0 && $nodef!=1)
      {
      $mod_def_start  = ($filename =~ /\.(mmp)$/i )?"\n#if defined $mod_def_name\n":"#ifdef  $mod_def_name\n";
      $mod_def_else = "\n#else  //$mod_def_name\n";
      $mod_def_end  = "\n#endif //$mod_def_name\n";
      }

    #PREPARE SEARCH
    $search =~ s/\s+/\\s\*/g;
    $search =~ s/([().+?\/])/\\$1/g;
    $search =~ s/_ANY_/\)\(\(\.\|\\s\)\*\?\)\(/g;
    $mod_text=~ s/\s+/\\s\*/g;
    $mod_text=~ s/([().+?\/])/\\$1/g;

    #AFTER ANY
    if("afterany" eq $operation)
      {
      $changenote .= "(added following)\n";
      if($data =~ /($search)/ )
        {
        $mod = ($ifdefs!=0 && $nodef!=1)?"$mod_def_start$replace$mod_def_end":"$replace";
        $data =~ s/($search)/$1$2$changenote$mod\n$4/g;
        }
      else
        {
        print "    -- not found:\n     $search";
        }
      }
    #REPLACE
    elsif("replace" eq $operation)
      {
      $changenote .= "(replacement)\n";
      if($data =~ /($search)/ )
        {
        $mod = ($ifdefs!=0 && $nodef!=1)?"$mod_def_start$replace$mod_def_else$1$mod_def_end":"$replace";
        $data =~ s/($search)/$changenote$mod\n/g;
        }
      else
        {
        print "    -- not found ($operation):\n     $search";
        }
      }

    #COMMENT
    elsif("comment" eq $operation)
      {
      $changenote .= "(commented following)\n";
      if($data =~ /($search)/ )
        {
        $commented = $1;
        $comm = $commented;
        $commented =~ s/\n/\n$linecomment/g;
        $mod = ($ifdefs!=0 && $nodef!=1)?"$mod_def_start$linecomment$commented$mod_def_else$comm$mod_def_end":"$linecomment$commented";
        $data =~ s/($search)/$changenote$mod/g;
        }
      else
        {
        print "    -- not found:\n     \"$search\"\n";
        }
      }
    #UNCOMMENT
    elsif("uncomment" eq $operation)
      {
      $changenote .= "(uncommented following)\n";
      if($data =~ /([\s#\/]*$search)/ )
        {
        $uncommented = $1;
        $uncommented =~ s/(^\s*$linecomment|(\n)\s*$linecomment)/$2/g;
        $mod = ($ifdefs!=0 && $nodef!=1)?"$mod_def_start$uncommented$mod_def_else$1$mod_def_end":"$uncommented";
        $data =~ s/([\s#\/]*$search)/$changenote$mod/g;
        }
      else
        {
        print "    -- not found:\n     \"$search\"\n";
        }
      }

    #ADD BELOW (add below search)
    elsif("addbelow" eq $operation)
      {
      $changenote .= "(added)\n";
      if($data =~ /($search)/ )
        {
        $mod = ($ifdefs!=0 && $nodef!=1)?"$mod_def_start$replace$mod_def_end":"$replace";
        $data =~ s/($search)/$1$changenote$mod\n/g;
        }
      else
        {
        print "    -- not found:\n     \"$search\"\n";
        }
      }
    #ADD TO TOP
    elsif("addtop" eq $operation)
      {
      $changenote .= "(added)\n";
      $mod = ($ifdefs!=0 && $nodef!=1)?"$mod_def_start$replace$mod_def_end":"$replace";
      $data = "$changenote$mod\n$data";
      }

    #UNKNOWN
    else
      {
      print "    -- unknown modification request: \"$operation\"\n";
      }

    $mod="";
    open( OUTFILE, "> $filepath" ) || die "  - BREAK: Cannot open output file, is it marked READ ONLY?.\n$!\n";
    print OUTFILE $data;
    close( OUTFILE );
    }
  else
    {
    print "    -- modification skipped\n";
    }
  print "    -- modification end\n";
  }

######### COPY FROM -> TO
sub CopyFiles
  {
    my($ref_from, $ref_to) = @_;    # Two
  local(@from)  = @{$ref_from};
  local(@to)    = @{$ref_to};
  local($size_from)   = scalar(@from);
  local($size_to)   = scalar(@to);

  if($size_from==0 || $size_to==0)
    {
    return ("     - no files to copy.\n");
    }

  local($i_from);
  for( $i_from=0; $i_from<$size_from; $i_from++ )
    {
    local($i_to);
    for( $i_to=0; $i_to<$size_to; $i_to++ )
      {
        #$fromall = join('+', @from);

        #make paths to correct format
        @from[$i_from]  =~ s/\//\\/g;
        @to[$i_to]    =~ s/\//\\/g;
        $command = "copy ".@from[$i_from]." ".@to[$i_to]." /-Y\n";

        #print $command;
        system($command) == 0 or return ("Copy Error.");
      }
    }
  print "     - COPY FINISHED\n";
  return 1;
  }


######### ACTIVATE SCRIPTS
sub ActivateScripts
  {
    my($ref_name, $ref_location) = @_;    # Two
  local(@name)    = @{$ref_name};
  local(@location)  = @{$ref_location};
  local($size_name)   = scalar(@name);
  local($size_location)   = scalar(@location);

  if($size_name==0 || $size_location==0)
    {
    return ("    - no scripts to activate.\n");
    }

  #COMMENT DEMO MODULE
  # -----------------------------------------------------------------------
  $scr_id = 100;
  $scr_file{$scr_id} = @location[0]; #'/epoc32/winscw/c/TestFramework/TestFramework.ini';
  $scr_oper{$scr_id} = 'comment';
  $scr_srch{$scr_id} =
  '\[New_Module\]
  ModuleName= demomodule
  \[End_Module\]';
  $scr_text{$scr_id} = '';
  $scr_nodf{$scr_id} = 1;      #NOKIATSY defs are not active
  $scr_id++;

  #COMMENT ADAPTATION TEST ENV
  # -----------------------------------------------------------------------
  $scr_file{$scr_id} = @location[0]; #'/epoc32/winscw/c/TestFramework/TestFramework.ini';
  $scr_oper{$scr_id} = 'comment';
  $scr_srch{$scr_id} =
'\[New_Module\]
ModuleName= adaptationtestenvstifadapter
\[End_Module\]';
  $scr_text{$scr_id} = '';
  $scr_nodf{$scr_id} = 1;      #NOKIATSY defs are not active
  $scr_id++;

  local($i_name);
  local($old_name) = "";
  local($activated) = "";

  for( $i_name=0; $i_name<$size_name; $i_name++ )
    {
    my($scr_dir, $scr_filename) = @name[$i_name] =~ m/(.*\/)(.*)$/;

    if(!(@name[$i_name] eq $old_name))
      {
      print "   - Do you want to add '".$scr_filename."' to the TestFramework.ini? [y/n] ";
      $key = &ReadKey();
      }
    if('y' eq $key)
      {
      $activated .= 'TestCaseFile= c:\\TestFramework\\'.$scr_filename."\n";
      }
    else
      {
      $activated .= '#TestCaseFile= c:\\TestFramework\\'.$scr_filename."\n";
      }
    $old_name = @from[$i_name];
    }

if(length($activated))
  {
  #ADD NEW MODULEs
  # -----------------------------------------------------------------------
  $scr_file{$scr_id} = @location[0]; #'/epoc32/winscw/c/TestFramework/TestFramework.ini';
  $scr_oper{$scr_id} = 'addbelow';
  $scr_srch{$scr_id} =
  '# name of the test module when adding new test module';
  $scr_text{$scr_id} =
'[New_Module]
ModuleName= testscripter
'.$activated.'[End_Module]';
  $scr_nodf{$scr_id} = 1;      #NOKIATSY defs are not active
  $scr_id++;
  }

  $mod_amount = $scr_id;

  #modify
  for($i_mods=100;$i_mods<$mod_amount;$i_mods++)
    {
    &Modify($scr_file{$i_mods}, $scr_oper{$i_mods}, $scr_srch{$i_mods}, $scr_text{$i_mods},$i_mods, $scr_nodf{$i_mods});
    }
  print "   - ACTIVATING FINISHED\n";
  return 1;
  }


######### Read a Key.
sub ReadKey
  {
  #READKEY
  $readkey=lc( getc(STDIN) );
  while($readkey =~ /([^a-z0-9])/ )
    {
    $readkey=lc( getc(STDIN) );
    }
  return substr( $readkey, 0, 1);
  }


######### CREATING A MODIFICATION LIST
#
# each MOD shoud have at least three of the following:
#
# 1. $mod_file{$mod_id}
#    - path to the file to be modified
#
# 2. $mod_oper{$mod_id}
#    - modification operation, currently supported:
#      "addbelow"  - adds text($mod_text) to $mod_file right after the $mod_srch -text
#      "replace"   - replaces instances of $mod_srch by $mod_text
#      "comment"   - puts a c-style comment to every line in $mod_srch
#      "afterany"  - adds text($mod_text) to $mod_file according to a keyword "_ANY_" in $mod_srch
#      "addtop"    - adds to top of the $mod_file
#
# 3. $mod_srch{$mod_id}
#    - search string(can be multiple lines)
#
# 4. $mod_text{$mod_id}
#    - text replacement or addition, depending on operation.
#
# 5. $mod_nodf{$mod_id}
#    - if the modification should _NOT_ be flagged. Use this only for special cases!
#
#
sub AddMods
  {
  $mod_id = 0;


  # -----------------------------------------------------------------------
  $mod_file{$mod_id} = $nokiatsy_path.'/tsy/nokiatsy_dll/group/nokiatsy_dll.mmp';
  $mod_oper{$mod_id} = 'replace';
  $mod_srch{$mod_id} = 'LIBRARY       commontsy.lib     // for using Common Tsy';
  $mod_text{$mod_id} = 'LIBRARY       NTT_core.lib     // for TEST tool';
  $mod_id++;

  # -----------------------------------------------------------------------
  $mod_file{$mod_id} = $nokiatsy_path.'/tsy/nokiatsy_dll/group/nokiatsy_dll.mmp';
  $mod_oper{$mod_id} = 'addbelow';
  $mod_srch{$mod_id} = 'UID           0x1000008D 0x1000A84A';
  $mod_text{$mod_id} =
  ' #define '.$mod_flag.'_MMP
  #if defined('.$mod_flag.'_MMP)
  MACRO '.$mod_flag.'
  #endif  //'.$mod_flag.'_MMP
  ';
  $mod_nodf{$mod_id} = 1;
  $mod_id++;


  #EPOC.INI (for running emulator in textshell)
  # -----------------------------------------------------------------------
  $mod_file{$mod_id} = '/epoc32/data/epoc.ini';
  $mod_oper{$mod_id} = 'addtop';
  $mod_srch{$mod_id} = '';
  $mod_text{$mod_id} = 'textshell';
  $mod_id++;

  # -----------------------------------------------------------------------
  $mod_file{$mod_id} = '/epoc32/data/epoc.ini';
  $mod_oper{$mod_id} = 'replace';
  $mod_srch{$mod_id} = 'configuration epoc_480x640.ini';
  $mod_text{$mod_id} = '// configuration epoc_480x640.ini';
  $mod_id++;

  # -----------------------------------------------------------------------
  $mod_file{$mod_id} = '/epoc32/data/epoc.ini';
  $mod_oper{$mod_id} = 'replace';
  $mod_srch{$mod_id} = 'configuration epoc_640x480.ini';
  $mod_text{$mod_id} = '// configuration epoc_640x480.ini';
  $mod_id++;


  #STIF SPEEDUP
  # -----------------------------------------------------------------------
  $mod_file{$mod_id} = $stifroot.'/stif/ConsoleUI/src/ConsoleUI.cpp';
  $mod_oper{$mod_id} = 'comment';
  $mod_srch{$mod_id} =
  '    RDebug::Print(_L("We have to wait until emulator is up & running"));
      User::After(40000000);';
  $mod_text{$mod_id} = '';
  $mod_nodf{$mod_id} = 1;      #NOKIATSY defs are not active in STIF
  $mod_id++;


  #TEST FRAMEWORK INI (create your own additions)
  # -----------------------------------------------------------------------
  #$mod_file = '/stif/Group/TestFrameWork.INI';
  #$mod_file = '/epoc32/data/Z/System/Data/TestFrameWork.INI';
  #$mod_file = '/epoc32/winscw/c/TestFramework/TestFrameWork.INI';
  #$mod_file = '/epoc32/wins/c/TestFramework/TestFrameWork.INI';
  #$mod_id++;

##################### ADD NEW MODIFICATIONS HERE ######################


  # NEW MOD TASK TEMPLATE
  #$mod_file{$mod_id} = '/filepath';
  #$mod_oper{$mod_id} = 'command'; #POSSIBLE KEYWORDS: ('afterany'|'replace'|'comment'|'addbelow')
  #$mod_srch{$mod_id} = 'searched text (multiline is ok)';
  #$mod_text{$mod_id} = 'text to be added (when relevant, otherwise leave empty)';
  #$mod_id++;

##################### END MODIFICATIONS HERE ######################
  return ($mod_id);
  }



########### END SUBROUTINES ###########

################### SCRIPT ENDS ###################