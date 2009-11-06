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
* Description:   Icon testing class for SimAtkTsy testtool. Provides 
*                functionality to load icon data and verify it's content.
*
*/



// INCLUDE FILES
#include "saticon.h"
#include "satfile.h"
#include <stiflogger.h>


// CONSTANTS

// Image description size of one mage in ef_img file
const TUint8 KImageDescriptionSize  = 9;

// Image header length for color icons
const TUint8 KColourHeaderLength    = 6;

// Image file id mask
const TUint8 KFileIdMask            = 0x4F;

_LIT( KLogRecord, "CSatIcon::CheckIcon - Number of icons %d, using icon %d" );
_LIT( KLogInfo1, "CSatIcon::CheckIcon - Width %d, Height %d, Coding 0x%x " );
_LIT( KLogInfo2, "CSatIcon::CheckIcon - FileId 0x%x OffSet 0x%04x Length %d" );
_LIT( KLogClut, "CSatIcon::CheckIcon - ClutOffset 0x%04x Length %d");


// -----------------------------------------------------------------------------
// CSatIcon::CSatIcon
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatIcon::CSatIcon
        (
        const RSat& aSat,
        const TPtrC aType,
        const TUint8 aRecordId
        ) : iSat( aSat ), iType( aType ), iRecordId( aRecordId ), iFile( NULL )
    {
    }
    

// -----------------------------------------------------------------------------
// CSatIcon::~CSatIcon
// Destructor
// -----------------------------------------------------------------------------
//
CSatIcon::~CSatIcon()
    {
    delete iFile;
    iFile = NULL;
    }
    

// -----------------------------------------------------------------------------
// CSatIcon::OpenFile
// Opens SIM reference data file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatIcon::OpenFile()
    {
    TInt ret( KErrNone );
    
    if ( !iFile )
        {
        TRAP( ret, iFile = new( ELeave )CSatFile );

        if ( iFile )
            {
            ret = iFile->OpenFile();
            }
        else
            {
            ret = KErrNotFound;
            }
        }
    
    return ret;
    }
    

// -----------------------------------------------------------------------------
// CSatIcon::CheckIcon
// Loads icon from SIM using TSY and verifies received data with reference data
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatIcon::CheckIcon
        (
        CStifLogger* aLog,
        const TUint8 aImageNumber
        )
    {
    TInt ret( OpenFile() );
    
    aLog->Log( _L( "CSatIcon::CheckIcon" ) );
        
    if ( KErrNone != ret )
        {
        aLog->Log( _L( "CSatIcon::CheckIcon - ERROR \
            Could not open reference SIM data file!" ) );
        }
    else
        {
        //
        // 1) Read and verify Record data
        RSat::TIcon record;
        if ( KErrNone != ( ret = ReadRecord( record ) ) )
            {
            aLog->Log( _L( "CSatIcon::CheckIcon - ERROR \
                Reading of record failed!" ) );
            }
        else
            {
            ret = CheckRecord( aLog, record );
            }
        
        //
        // 2) Read and verify requested instance info from received record
        if ( KErrNone == ret )
            {
            aLog->Log( KLogRecord, record[0], aImageNumber );

            if ( KErrNone != ( ret = ReadInstanceInfo(
                record, aImageNumber ) ) )
                {
                aLog->Log( _L( "CSatIcon::CheckIcon - ERROR \
                    Reading of instance info failed!" ) );
                }
            else
                {
                aLog->Log( KLogInfo1, iInfo.iImgWidth, iInfo.iImgHeight,
                    iInfo.iCoding );
                aLog->Log( KLogInfo2, iInfo.iFileId, iInfo.iOffset,
                    iInfo.iLength );
                
                // Return value is ignored - record content has already been
                // verified.
                CheckInstanceInfo( aLog );
                }
            }
            
        //
        // 3) Read and verify image instance
        if ( KErrNone == ret )
            {
            HBufC8* imageData = HBufC8::New( iInfo.iLength );
            
            if ( !imageData )
                {
                ret = KErrNoMemory;
                }
            else
                {
                TPtr8 imagePtr( imageData->Des() );
                
                if ( KErrNone != ( ret = ReadImage( imagePtr ) ) )
                    {
                    aLog->Log( _L( "CSatIcon::CheckIcon - ERROR \
                        Reading of image instance failed!" ) );
                    }                
                // Verify image data
                else if ( KErrCorrupt == CheckImage( aLog, imagePtr ) )
                    {
                    ret = KErrCorrupt;
                    }
                }
            }
            
        //
        // 4) Read and verify clut data if applicaple
        if ( KErrNone == ret && RSat::KColour == iInfo.iCoding )
            {
            if ( KErrNone != ReadClutInfo() )
                {
                aLog->Log( _L( "CSatIcon::CheckIcon - ERROR \
                    Could not determine the clut offset and length!" ) );
                ret = KErrCorrupt;
                }
            else
                {
                aLog->Log( KLogClut, iClutInfo.iOffset, iClutInfo.iLength );
                
                HBufC8* clutData = HBufC8::New( iClutInfo.iLength );
                
                if ( !clutData )
                    {
                    ret = KErrNoMemory;
                    }
                else
                    {
                    TPtr8 clutPtr( clutData->Des() );
                    
                    if ( KErrNone != ( ret = ReadClut( clutPtr ) ) )
                        {
                        aLog->Log( _L( "CSatIcon::CheckIcon - ERROR \
                            Reading of clut failed!" ) );
                        ret = KErrCorrupt;
                        }                
                    // Verify image data
                    else if ( KErrCorrupt == CheckClut( aLog, clutPtr ) )
                        {
                        ret = KErrCorrupt;
                        }
                    }
                }
            }            
        }        
        
    return ret;
    }

    
// -----------------------------------------------------------------------------
// CSatIcon::ReadRecord
// Reads a record from EF_Img elementary file from the TSY
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatIcon::ReadRecord
        (
        RSat::TIcon& aRecord
        )
    {
    TRequestStatus status;
    
    iSat.GetIcon( status, iRecordId, aRecord );
    User::WaitForRequest( status );
    
    return status.Int();
    }

    
// -----------------------------------------------------------------------------
// CSatIcon::CheckRecord
// Verify the content of read record against reference SIM data
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatIcon::CheckRecord
        (
        CStifLogger* aLog,
        const RSat::TIcon& aRecord
        )
    {
    TInt ret( KErrNone );
    
    if ( KErrNone != iFile->FindFile( iType, RSat::KImgEf, iRecordId ) )
        {
        aLog->Log( _L( "CSatIcon::CheckRecord - ERROR \
            Reference record not found!" ) );        
        ret = KErrNotFound;
        }
    else if ( KErrNone != iFile->MatchData( aRecord ) )
        {
        aLog->Log( _L( "CSatIcon::CheckRecord - ERROR \
            Content of record did not match with reference!" ) );        
        ret = KErrCorrupt;
        }    
    
    return ret;
    }

    
// -----------------------------------------------------------------------------
// CSatIcon::ReadInstanceInfo
// Parse instance requested info structure from record data
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatIcon::ReadInstanceInfo
        (
        const RSat::TIcon& aRecord,
        const TUint8 aImageNumber
        )
    {
    TInt ret( KErrNone );
    
    // Clear current content
    iInfo = RSat::TInstanceInfoV3();
    
    // Ef_img record:
    //
    // Byte(s)  Description
    // ====================
    // 1        Number of Actual Image Instances
    // 2        Image 1 Instance Width
    // 3        Image 1 Instance Height
    // 4        Image 1 Coding Scheme (11 or 21)
    // 5 & 6    Image 1 Instance File Identifier (ef file)
    // 7 & 8    Offset into Image 1 Instance File
    // 9 & 10   Length of Image 1 Instance Data
    //
    // 11       Image 2 Instance Width
    // 12       Image 2 Instance Height
    // ...
    
    if ( !aRecord.Length() )
        {
        // Zero length record
        ret = KErrNotFound;
        }
    else if ( aImageNumber > aRecord[0] )
        {
        // There's not enough images in the record
        ret = KErrNotFound;
        }
    else
        {
        // Index point to start of the instance info in a record (=width)
        TUint index( ( aImageNumber - 1 ) * KImageDescriptionSize + 1 );

        if ( KImageDescriptionSize + index <= aRecord.Length() )
            {
            // No danger of over flow and recond contain icon descriptions
            iInfo.iImgWidth = aRecord[index++];
            iInfo.iImgHeight = aRecord[index++];
            iInfo.iCoding = static_cast<RSat::TImageCoding>( aRecord[index++] );
            iInfo.iFileId = ( aRecord[index++] << 8 ) + aRecord[index++];
            iInfo.iOffset = ( aRecord[index++] << 8 ) + aRecord[index++];
            iInfo.iLength = ( aRecord[index++] << 8 ) + aRecord[index++];
            
            // Both iInstanceNumber and iFileId seem to contain the file identifier
            iInfo.iInstanceNumber = iInfo.iFileId;
            }
        else
            {            
            ret = KErrNotFound;
            }        
        }
        
    // Basic check for obvious errors. File id should be: 4F XX
    if ( KFileIdMask != ( iInfo.iFileId >> 8 ) ) // 0x4F
        {
        ret = KErrNotFound;
        }
    
    return ret;    
    }

    
// -----------------------------------------------------------------------------
// CSatIcon::CheckInstanceInfo
// Verify the validity of read instance info
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatIcon::CheckInstanceInfo
        (
        CStifLogger* aLog
        )
    {
    TInt ret( KErrNone );    
        
    // Verify file Id
    if (  iInfo.iInstanceNumber != iInfo.iFileId )
        {
        aLog->Log( _L( "CSatIcon::CheckInstanceInfo - Warning \
            iInstanceNumber != iFileId!" ) );  
        ret = KErrCorrupt;      
        }
    else if ( iInfo.iFileId >> 8 != KFileIdMask )
        {
        aLog->Log( _L( "CSatIcon::CheckInstanceInfo - Warning \
            Instance file does not begin with 0x4F!" ) );        
        ret = KErrCorrupt;      
        }
        
    // Verify color coding
    if ( iInfo.iCoding != RSat::KBasic && iInfo.iCoding != RSat::KColour )    
        {
        aLog->Log( _L( "CSatIcon::CheckInstanceInfo - Warning \
            Coding corrupted!" ) );
        ret = KErrCorrupt;      
        }
        
    // Verify lenght
    if ( !iInfo.iLength )
        {
        aLog->Log( _L( "CSatIcon::CheckInstanceInfo - Warning \
            Image length is zero!" ) );
        ret = KErrCorrupt;      
        }
    
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatIcon::ReadImage
// Read Img-elementary file from the SIM
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatIcon::ReadImage
        (
        TDes8& aImageData                   // Image data output
        )
    {
    TInt ret( KErrNone );    

    if ( iInfo.iLength > aImageData.MaxLength() )
        {
        // Data won't fit!
        ret = KErrCorrupt;
        }
    else
        {
        TRequestStatus status;
        RSat::TInstanceInfoV3Pckg infoPckg( iInfo );
        
        iSat.GetImageInstance( status, infoPckg, aImageData );
        User::WaitForRequest( status );
        ret = status.Int();
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSatIcon::CheckImage
// Verify the content of read image data against reference SIM data
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatIcon::CheckImage
        (
        CStifLogger* aLog,
        const TDes8& aImageData
        )
    {
    TInt ret( KErrNone );
    
    // Check data lenght. Calculate length from image header.
    
    TInt i( 0 );
    
    // Width * heigth
    TInt lenInBits( aImageData[i++] * aImageData[i++] );
    if (  RSat::KColour == iInfo.iCoding )
        {
        // Include color depth as well for color icons
        lenInBits *=  aImageData[i];
        }

    TInt lenInBytes( lenInBits / 8 );

    // Round up if needed
    if ( lenInBits % 8 )
        {
        lenInBytes++;
        }
        
    // Add header length (2 for B/W and 6 for color icon)
    lenInBytes += RSat::KBasic == iInfo.iCoding ? 2 : 6;
            
    if ( iInfo.iLength != lenInBytes )            
        {
        aLog->Log( _L( "CSatIcon::CheckImage- Warning \
            Lenght does not match with the calculated one!" ) );
        
        // Use calculated to enable further testing
        iInfo.iLength = lenInBytes;
        }
    
    // Proceed with image data verification    
    if ( iInfo.iLength != aImageData.Length() )
        {
        aLog->Log( _L( "CSatIcon::CheckImage - ERROR \
            Unexpected icon data length!" ) );
        ret = KErrCorrupt;
        }
    else if ( KErrNone != iFile->FindFile( iType, iInfo.iFileId ) )
        {
        aLog->Log( _L( "CSatIcon::CheckImage - ERROR \
            Reference image instance file not found!" ) );        
        ret = KErrNotFound;
        }
        
    // Image data has to be verified in two parts because SimAtkTsy
    // does not return correct CLUT offset for color images.
    if ( KErrNone == ret )
        {
        if ( RSat::KBasic == iInfo.iCoding )
            {
            ret = iFile->MatchData( aImageData, iInfo.iOffset );
            }
        else
            {
            // Exclude clut offset ( 2 bytes )
            TPtrC8 ptr( aImageData.Left( KColourHeaderLength - 2 ) );
            ret = iFile->MatchData( ptr, iInfo.iOffset );
            
            if ( KErrNone == ret)
                {
                // Header was OK. Verify data after the header
                ptr.Set( aImageData.Mid( KColourHeaderLength ) );
                ret = iFile->MatchData(
                     ptr, iInfo.iOffset + KColourHeaderLength );
                }
            }

        if ( KErrNone != ret )
           {
            aLog->Log( _L( "CSatIcon::CheckImage - ERROR \
                Content of image instance did not match with reference!" ) );
            ret = KErrCorrupt;
            }
        }
        
    return ret;
    }


// -----------------------------------------------------------------------------
// CSatIcon::ReadClutInfo
//  Reads clut's offset and length from the scripted file
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatIcon::ReadClutInfo()
    {
    TInt ret( KErrNotFound );
    
    iClutInfo.iOffset = 0;
    iClutInfo.iLength = 0;

    // Image instance data returned by SimAtk does not contain
    // valid offsets for clut offset. Read it from reference file.
    TBuf8<KColourHeaderLength> imageHeader;
    
    if ( KErrNone == iFile->FetchData( imageHeader, iInfo.iOffset ) )
        {

        if ( imageHeader.Length() == imageHeader.MaxLength() ) 
            {
            // Header was read successfully
        
            // Number of clut entries is found from the image header indexes 4 & 5
            iClutInfo.iOffset = ( imageHeader[4] << 8 ) + imageHeader[5];

            // Clut length is three * number of clut entries
            // Number of clut entries is found from the image header
            // at index 3.
            iClutInfo.iLength = imageHeader[3] * 3;
            
            // 3GPP TS 11.11 V8.12.0: The value 0 shall be interpreted as 256
            if ( !iClutInfo.iLength )
                {
                iClutInfo.iLength = 256 * 3;
                }
            
            ret = KErrNone;
            }
        }
        
    return ret;
    }
    

// -----------------------------------------------------------------------------
// CSatIcon::ReadClut
// Reads clut informartion from the SIM for current image
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatIcon::ReadClut
        (
        TDes8& aClutData                   // Clut data output
        )
    {
    TRequestStatus status;
    
    iSat.GetClut( status, iInfo.iInstanceNumber, iInfo.iOffset, aClutData );
    User::WaitForRequest( status );
    
    return status.Int();
    }    


// -----------------------------------------------------------------------------
// CSatIcon::CheckClut
// Verify the content of read clut data against reference SIM data
// (detailed description in header file)
// -----------------------------------------------------------------------------
//
TInt CSatIcon::CheckClut
        (
        CStifLogger* aLog,
        const TDes8& aClutData
        )
    {
    TInt ret( KErrNone );
    
    // Check data lenght
    if ( iClutInfo.iLength != aClutData.Length() )
        {
        aLog->Log( _L( "CSatIcon::CheckClut - ERROR \
            Unexpected clut data length!" ) );
        ret = KErrCorrupt;
        }
    else if ( KErrNone != iFile->MatchData( aClutData, iClutInfo.iOffset ) )
        {
        aLog->Log( _L( "CSatIcon::CheckClut - ERROR \
            Content of clut data  did not match with reference!" ) );
        ret = KErrCorrupt;
        }
    
    return ret;
    }    

// End of File    
