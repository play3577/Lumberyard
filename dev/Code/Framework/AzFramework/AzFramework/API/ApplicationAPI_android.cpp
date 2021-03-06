/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#include "ApplicationAPI.h"
#include "ApplicationAPI_android.h"

namespace AzFramework
{
    class ApplicationLifecycleEventsHandler::Pimpl
        : public AndroidLifecycleEvents::Bus::Handler
    {
    public:
        AZ_CLASS_ALLOCATOR(Pimpl, AZ::SystemAllocator, 0);

        Pimpl()
            : m_lastEvent(ApplicationLifecycleEvents::Event::None)
        {
            AndroidLifecycleEvents::Bus::Handler::BusConnect();
        }

        ~Pimpl() override
        {
            AndroidLifecycleEvents::Bus::Handler::BusDisconnect();
        }

        void OnPause() override
        {
            EBUS_EVENT(ApplicationLifecycleEvents::Bus, OnApplicationConstrained, m_lastEvent);
            m_lastEvent = ApplicationLifecycleEvents::Event::Constrain;
            EBUS_EVENT(ApplicationLifecycleEvents::Bus, OnApplicationSuspended, m_lastEvent);
            m_lastEvent = ApplicationLifecycleEvents::Event::Suspend;
        }

        void OnResume() override
        {
            EBUS_EVENT(ApplicationLifecycleEvents::Bus, OnApplicationResumed, m_lastEvent);
            m_lastEvent = ApplicationLifecycleEvents::Event::Resume;
            EBUS_EVENT(ApplicationLifecycleEvents::Bus, OnApplicationUnconstrained, m_lastEvent);
            m_lastEvent = ApplicationLifecycleEvents::Event::Unconstrain;
        }

        void OnDestroy() override
        {
            EBUS_EVENT(ApplicationLifecycleEvents::Bus, OnMobileApplicationWillTerminate);
        }

        void OnLowMemory() override
        {
            EBUS_EVENT(ApplicationLifecycleEvents::Bus, OnMobileApplicationLowMemoryWarning);
        }

    private:
        ApplicationLifecycleEvents::Event m_lastEvent;
    };

    ApplicationLifecycleEventsHandler::ApplicationLifecycleEventsHandler()
        : m_pimpl(aznew Pimpl())
    {
    }

    ApplicationLifecycleEventsHandler::~ApplicationLifecycleEventsHandler()
    {
        delete m_pimpl;
    }
} // namespace AzFramework
