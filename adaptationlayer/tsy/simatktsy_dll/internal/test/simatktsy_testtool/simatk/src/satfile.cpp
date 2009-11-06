/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides functionality to open SIM data file and
*                to make searches and comperations to its content
*                Each line in SIM data file represent a separate elementary file 
*                or file's record (referenced as RefEfs).
*
*/



// INCLUDE FILES
#include "satfile.h"


// CONSTANTS

// Name and location of SIM data file
_LIT( KSimDataFile, "\\TestFramework\\SimAtkSimData.ini" );

// Special characters found from data file
const TUint8 KCharEndOfFile         = 0;
const TUint8 KCharEndOfLine         = '\n';
const TUint8 KCharCarriageReturn    = '\r';
const TUint8 KCharTabulator         = '\t';
const TUint8 KCharSpace             = ' ';
const TUint8 KCharComment           = '#';

// State machine for locating data from SIM data file
enum TSearchState
    {
    EType,      // Type of SIM (SATTS/GCF)
    EFile,      // File Id ( for example 4F21)
    ERecord     // Record Id. Some files may contain several records.
    };
    

// DATA TYPES
typedef TBuf8<1> TLetter;


// -----------------------------------------------------------------------------
// CSatFile::CSatFile
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatFile::CSatFile() : iSimFilePos( KErrNotFound )
    {
    }


// -----------------------------------------------------------------------------
// CSatFile::~CSatFile
// Destructor
// -----------------------------------------------------------------------------
//
CSatFile::~CSatFile()
    {
    if ( KErrNotFound != iSimFilePos )
        {
        iFile.Close();
        iFileServer.Close();
        }
    }


// -----------------------------------------------------------------------------
// CSatFile::OpenFile
// Opens SIM data file
// (other items were commented in a header). 
// -----------------------------------------------------------------------------
//
TInt CSatFile::OpenFile()
    {
    TInt ret( KErrNotFound );

    // Connect to file server and open file
    if ( KErrNone == iFileServer.Connect() )
        {
        if ( KErrNone == iFile.Open(
             iFileServer, KSimDataFile, EFileRead ) )
            {
            iSimFilePos = 0;
            ret = KErrNone;
            }
        else
            {
            // Opening of file failed.
            iFileServer.Close();
            }
        }
    
    return ret;   
    }


// -----------------------------------------------------------------------------
// CSatFile::FindFile
// Search specified REfEf from from the SIM data file
// (other items were commented in a header). 
// -----------------------------------------------------------------------------
//
TInt CSatFile::FindFile
        (
        const TPtrC aSimType,
        const TInt aFileId,
        const TInt aRecord
        )
    {
    TInt ret( KErrNone );
    
    // Check the file is opened successfully
    if ( KErrNotFound != iSimFilePos )
        {
        // Data read from file is 8-bit. Convert paramerters to string 
        // presentation for comparation purposes.
        TBuf8<3> type;
        TBuf8<4> fileId;
        TBuf8<2> record;
        
        type.Append( aSimType.Left( 
            Min( aSimType.Length(), type.MaxLength() ) ) );
        fileId.Num( aFileId, EHex );
        
        // Not all elementary files have records
        if ( aRecord)
            {
            record.Num( aRecord );
            }
        
        // Rewind to the beginning of SIM data file
        iSimFilePos = 0;
        iFile.Seek( ESeekStart, iSimFilePos );
        
        // Begin search by looking for line which starts with correct
        // SIM type keyword (SATTS/GCF)
        TSearchState lookingFor( EType );
        
        TBuf8<10> keyWord;        
        do
            {
            // Get next token from file
            if ( KErrNone == ( ret = NextToken( keyWord ) ) )
                {
                // Search state machine. Next action depens on what
                // we're looking for now.
                switch( lookingFor )
                    {
                    case EType: // Looking for line with correct SIM type
                        {
                        if ( KErrNone == type.CompareC( keyWord ) )
                            {
                            // Type matches, check file id next
                            lookingFor = EFile;
                            }
                        else
                            {
                            // Wrong type. Continue looking from next line.
                            lookingFor = EType;
                            ret = NextLine();
                            }
                        break;
                        }
                    case EFile: // Looking for line with correct file Id
                        {
                        if ( KErrNone == fileId.CompareC( keyWord ) )
                            {
                            // Correct file. If record was provided
                            // continue to look correct record id. 
                            // Otherwise correct line was found.  
                            if ( !record.Length() )
                                {
                                // Line found, store lines starting position
                                iFile.Seek( ESeekCurrent, iSimFilePos );
                                }
                            else
                                {
                                lookingFor = ERecord;
                                }
                            }
                        else
                            {
                            // Wrong file. Continue looking from next line.
                            lookingFor = EType;
                            ret = NextLine();
                            }
                        break;
                        }
                    case ERecord: // Looking for line with correct record id
                        {
                        if ( KErrNone == record.CompareC( keyWord ) )
                            {
                            // Line found, store lines starting position
                            iFile.Seek( ESeekCurrent, iSimFilePos );
                            }
                        else
                            {
                            // Wrong record. Continue looking from next line.
                            lookingFor = EType;
                            ret = NextLine();
                            }
                            
                        break;
                        }
                    default:
                        {
                        // This should be impossible to reach
                        lookingFor = EType;
                        break;
                        }
                    }
                }
            
            // Continue search until the line containing RefEf is found
            // or parsing of file fails (=end of file is reached)
            } while ( KErrNone == ret && 0 == iSimFilePos );
        }
    
    return ret;
    }
    
    
// -----------------------------------------------------------------------------
// CSatFile::MatchData
// Compares provided data to the RefEf
// (other items were commented in a header). 
// -----------------------------------------------------------------------------
//
TInt CSatFile::MatchData
        (
        const TDesC8& aData,
        const TUint aOffset
        )
    {
    // Skip to offset
    TInt ret( Goto( aOffset ) );
    
    if ( KErrNone == ret )
        {
        TUint value;
        TUint dataLength( aData.Length() );

        // Compare data byte by byte to the RefEf
        for ( TUint i ( 0 ) ; i < dataLength && KErrNone == ret ; i++ )
            {
            if ( KErrNone == ( ret = NextValue( value ) ) )
                {
                if ( aData[i] != value )
                    {
                    // Data mismatch
                    ret = KErrCorrupt;
                    }
                }
            else
                {
                // Getting next byte from RefEf failed.
                ret = KErrOverflow;
                }
            }
        }
   
    return ret;
    }
    
    
// -----------------------------------------------------------------------------
// CSatFile::FetchData
// Read data from RefEf
// (other items were commented in a header). 
// -----------------------------------------------------------------------------
//
TInt CSatFile::FetchData
        (
        TDes8& aData,
        const TUint aOffset
        )
    {
    // Leap to offset
    TInt ret( Goto( aOffset ) );
    
    TUint dataMax( aData.MaxLength() );
    
    aData.Zero();
    
    if ( KErrNone == ret )
        {
        TUint value( 0 );
        do
            {
            // Append values to the output until max capacity is reached
            // or parsing of next value fails.
            if ( KErrNone ==  ( ret = NextValue( value ) ) )
                {
                aData.Append( value );
                }
            } while ( KErrNone == ret && aData.Length() < dataMax );
        }
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatFile::NextChar
// Returns next character from a data file
// (other items were commented in a header). 
// -----------------------------------------------------------------------------
//
TInt CSatFile::NextChar
        (
        TChar& aChar
        )
    {
    TInt ret( KErrNone );
    
    aChar = KCharEndOfFile;
    
    // Data from file is read to a buffer
    TLetter letter;
    
    do
        {
        iFile.Read( letter );
        
        if ( letter.Length() )
            {
            // Letter was read successfully. Convert to char
            aChar = letter[0];
            
            // Prohibit stepping beyond end of line
            if ( KCharEndOfLine == aChar )
                {
                // Line end. Step back
                TInt stepBack( -1 );
                iFile.Seek( ESeekCurrent, stepBack );
                
                ret = KErrNotFound;
                }
            }
        else
            {
            // End of File (length of read letter is zero)
            ret = KErrNotFound;
            }
        
        // Skip carriage return (\r) in end of line sequece (\r\n)
        } while ( KCharCarriageReturn == aChar && KErrNone == ret );
    
    return ret;   
    }
    
    
// -----------------------------------------------------------------------------
// CSatFile::NextLine
// Skips the current line 
// (other items were commented in a header). 
// -----------------------------------------------------------------------------
//
TInt CSatFile::NextLine()
    {
    TInt ret( KErrNone );

    TChar ch( KCharEndOfLine );
    
    // Read characters until end of line is reached
    do
        {
        ret = NextChar( ch );
        } while ( KErrNone == ret );
        
    // Read one letter, that should should be (\n).
    TLetter letter;
    iFile.Read( letter );

    if ( !letter.Length() )
        {
        // This is the end of File
        ret = KErrNotFound;
        }
    else
        {
        // Reading position is in the beginning of next line
        ret = KErrNone;
        }

    return ret;
    }
    
    
// -----------------------------------------------------------------------------
// CSatFile::NextToken
// Read next token (word or number) from a file
// (other items were commented in a header). 
// -----------------------------------------------------------------------------
//
TInt CSatFile::NextToken
        (
        TDes8& aToken
        )
    {
    TInt ret( KErrNone );

    TBool done( EFalse );

    TChar ch( KCharEndOfLine );

    TUint tokenMax( aToken.MaxLength() );
    
    aToken.Zero();

    do
        {
        // Read next char
        ret = NextChar( ch );
        
        // Next action depends on the type of character
        switch( ch )
            {
            case KCharComment: // Comment charater received
                {                
                aToken.Zero();
                // Decision: No comment char allowed at all and
                // whole line is skipped
                if ( KErrNone != NextLine() )
                    {
                    // cound not find next line (end of file)
                    ret = KErrNotFound;
                    done = ETrue;
                    }
                break;
                }
            case KCharSpace: // These characters can end the reading of tokens
            case KCharTabulator:
            case KCharEndOfLine:
            case KCharEndOfFile:
                {
                if ( aToken.Length() )
                    {
                    // Some characters have been succesfully read to output.
                    // Token was read successfully.
                    ret = KErrNone;
                    done = ETrue;
                    }
                else if ( KCharEndOfLine == ch )
                    {
                    // Output is still empty and end of line has been reached.
                    // Skip line
                    if ( KErrNone != NextLine() )
                        {
                        // No next line, end of file reached
                        ret = KErrNotFound;
                        done = ETrue;
                        }                    
                    }
                else if ( KCharEndOfFile == ch )
                    {
                    // End of file was reached.
                    ret = KErrNotFound;
                    done = ETrue;
                    }
                break;
                }
            default:
                {
                // Not a special character. Append to output if valid 
                // alphabet or digit.
                if ( ch.IsAlphaDigit() )
                    {
                    if ( aToken.Length() < tokenMax )
                        {
                        aToken.Append( ch );
                        }
                    else
                        {
                        // Too long token parsed.
                        ret = KErrOverflow;
                        done = ETrue;
                        }
                    }
                else
                    {
                    // Chars that aren't either digits or alphabets are ignored
                    }
                break;
                }
            }
        
        } while ( !done );
    
    return ret;
    }
    
    
// -----------------------------------------------------------------------------
// CSatFile::NextValue
// Parses next hex integer value from RefEf
// (other items were commented in a header). 
// -----------------------------------------------------------------------------
//
TInt CSatFile::NextValue
        (
        TUint& aValue
        )
    {
    TInt ret( KErrNone );
    
    TBuf8<2> valueString; // 8-bit max
    
    // Read value and convert to integer. Make also sure that
    // RefEf has been located beforehand
    if ( KErrNone == NextToken( valueString ) && 0 < iSimFilePos  )
        {
        TLex8 lex( valueString );
        
        if ( KErrNone != lex.Val( aValue, EHex ) )
            {
            ret = KErrNotFound;
            }
        }
    else
        {
        ret = KErrNotFound;
        }

    return ret;
    }
    
    
// -----------------------------------------------------------------------------
// CSatFile::Goto
// Forward to offset in the RefEf
// (other items were commented in a header). 
// -----------------------------------------------------------------------------
//
TInt CSatFile::Goto
        (
        const TUint aOffset
        )
    {
    TInt ret( KErrNone );
    
    // Make sure that RefEf has been seached beforehand
    if ( 0 < iSimFilePos )
        {
        // First go to the start of RefEf
        
        // Get current position
        TInt posNow( 0 );
        iFile.Seek( ESeekCurrent, posNow );
        
        // Rewind to RefEf start
        if ( posNow != iSimFilePos )
            {
            TInt posDelta( iSimFilePos - posNow );
            iFile.Seek( ESeekCurrent, posDelta );
            }
        
        // Skip offsets if provided
        if ( aOffset )
            {
            TUint i( aOffset );
            TUint value( 0 ); // Not needed
            
            while ( i && KErrNone == ret )
                {
                ret = NextValue( value );
                i--;
                }
            }
        }
    else
        {
        // File position not found
        ret = KErrNotReady;
        }
    
    return ret;    
    }


// End of File
