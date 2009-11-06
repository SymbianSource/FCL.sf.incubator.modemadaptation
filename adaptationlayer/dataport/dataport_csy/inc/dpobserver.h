/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef DPOBSERVER_H
#define DPOBSERVER_H

//  INCLUDES

// LOCAL CONSTANTS
// none


// DATA TYPES
// none

// FORWARD DECLARATIONS
class CDpSubject;

// CLASS DECLARATION

/**
*  CDpObserver is observer base class.
*  Observers, which need state information, should be derived from this class.
*/
class CDpObserver : CBase
    {
    public:  // Methods

	// Constructors and destructor
        
        /**
        * Destructor.
        */
        ~CDpObserver();

    public:// New methods

        /**
        * This method updates observer.
        * @param aChangedSubject Subject, which updates observer
        */
        virtual void UpDate( CDpSubject* aChangedSubject ) = 0;
        

    protected:  // Methods

    // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CDpObserver();

    public:     // Data
        // Offset for singly linked list        
        static const TInt   iOffset;

    private:    // Data
		/** Singly linked list. */
        TSglQueLink        iSlink;



    };

/**
*  CDpSubject is subject base calss.
*  Subjects, of which state information is needed to be distributed to
*  observers, should be derived from from this class.
*/
class CDpSubject : public CBase
    {
    public:  // Constructors and destructor
      
        /**
        * C++ default constructor.
        */
        CDpSubject();

        /**
        * Destructor.
        */
        ~CDpSubject();

    public:  // New methods

        /**
        * Attach observer to this subject.
        * @param aObserver Observer
        */
        void Attach( CDpObserver& aObserver );

        /**
        * Detach observer from this subject
        * @param aObserver Observer
        */
        void Detach( CDpObserver& aObserver );

        /**
        * Notify all observers of this subject object.
        */
        void Notify();

    protected:  // Methods
        //None

    private:  // Data

		// Observer list
        TSglQue<CDpObserver> iObserverList;

        // Iterator for observer list
        TSglQueIter<CDpObserver> iObserverIter;

    };

#endif      // DPOBSERVER_H

//  End of File
