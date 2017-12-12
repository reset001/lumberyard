/*
* All or Portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
#pragma once

#include <qpoint.h>

#include <AzCore/Component/EntityId.h>
#include <AzCore/EBus/EBus.h>
#include <AzCore/std/string/string.h>
#include <AzCore/Math/Vector2.h>

#include <GraphCanvas/Types/Endpoint.h>
#include <GraphCanvas/Types/Types.h>
#include <GraphCanvas/Types/TranslationTypes.h>

class QGraphicsLayoutItem;

namespace AZ
{
    class any;
}

namespace GraphCanvas
{
    //! The type of connections that the slot specifies
    //! Currently we only support input/output for creating connections.
    enum ConnectionType
    {
        CT_None = 0,
        CT_Input,
        CT_Output,

        CT_Invalid = -1
    };

    typedef AZ::Crc32 SlotType;

    namespace SlotTypes
    {
        static const SlotType Invalid = AZ::Crc32();

        static const SlotType DataSlot = AZ_CRC("SlotType_Data", 0x85691298);

        static const SlotType ExecutionSlot = AZ_CRC("SlotType_Execution", 0xfe5e03a7);

        static const SlotType PropertySlot = AZ_CRC("SlotType_Property", 0xccaefd85);

        static const SlotType VariableReferenceSlot = AZ_CRC("SlotType_Variable", 0x8b1166d6);
    }

    // Visual Identification of how the Slot should be grouped for display
    typedef AZ::Crc32 SlotGroup;

    namespace SlotGroups
    {
        static const SlotGroup Invalid = AZ::Crc32();

        // Slot Group used by default for Data Slots
        static const SlotGroup DataGroup = AZ_CRC("SlotGroup_Data", 0xa0e29e91);

        // Slot Group used by default for Execution Slots
        static const SlotGroup ExecutionGroup = AZ_CRC("SlotGroup_Execution", 0x51524cd3);

        // Slot Group used by default for Property Slots
        static const SlotGroup PropertyGroup = AZ_CRC("SlotGroup_Property", 0xcafac52c);

        // Slot Group used by default for Variable Slots
        static const SlotGroup VariableReferenceGroup = AZ_CRC("SlotGroup_VariableReference", 0x7508f93b);

        // Slot Group used by default for Variable Slots
        static const SlotGroup VariableSourceGroup = AZ_CRC("SlotGroup_VariableSource", 0x67fa9111);
    }

    struct SlotConfiguration
    {
    public:
        AZ_TYPE_INFO(SlotConfiguration, "{E080FC05-EEB6-47A6-B939-F62A45C2B1D2}");

        ConnectionType m_connectionType = ConnectionType::CT_Invalid;

        TranslationKeyedString m_tooltip = TranslationKeyedString();
        TranslationKeyedString m_name = TranslationKeyedString();
        SlotGroup m_slotGroup = SlotGroups::Invalid;
    };

    struct SlotGroupConfiguration
    {
    public:
        AZ_TYPE_INFO(SlotGroupConfiguration, "{88F7AB93-9F26-4059-BD37-FFBD41E38AF6}");
        AZ_CLASS_ALLOCATOR(SlotGroupConfiguration, AZ::SystemAllocator, 0);

        SlotGroupConfiguration()
            : m_layoutOrder(0)
        {
        }

        SlotGroupConfiguration(int layoutOrder)
            : m_layoutOrder(layoutOrder)
        {
        }

        int m_layoutOrder = 0;
        bool m_visible = true;
    };

    typedef AZStd::unordered_map< SlotGroup, SlotGroupConfiguration > SlotGroupConfigurationMap;
    
    class SlotGroupConfigurationComparator
    {
    public:
        SlotGroupConfigurationComparator()
            : m_slotConfigurationMap(nullptr)
        {
            AZ_Assert("GraphCanvas", "Invalid Slot Group Configuration Comparator");
        }

        explicit SlotGroupConfigurationComparator(const SlotGroupConfigurationMap* slotConfigurationMap)
            : m_slotConfigurationMap(slotConfigurationMap)
        {
        }

        bool operator()(const SlotGroup& a, const SlotGroup& b)
        {
            const SlotGroupConfiguration& configA = m_slotConfigurationMap->at(a);
            const SlotGroupConfiguration& configB = m_slotConfigurationMap->at(b);

            if (configA.m_layoutOrder == configB.m_layoutOrder)
            {
                return a < b;
            }
            else
            {
                return configA.m_layoutOrder < configB.m_layoutOrder;
            }
        }

    private:
        const SlotGroupConfigurationMap* m_slotConfigurationMap;
    };

    static const AZ::Crc32 kSlotServiceProviderId = AZ_CRC("GraphCanvas_SlotService", 0x701eaf6b);

    //! SlotRequests
    //! Requests to retrieve or modify the current state of a slot.
    class SlotRequests
        : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;
        using BusIdType = AZ::EntityId;

        //! Get the entity ID of the node that owns this slot, if any.
        virtual const AZ::EntityId& GetNode() const = 0;
        //! Set the entity ID of the node that owns this slot, if any.
        virtual void SetNode(const AZ::EntityId&) = 0;

        //! Get the name, or label, of the slot.
        //! These generally appear as a label against \ref Input or \ref Output slots.
        virtual const AZStd::string& GetName() const = 0;
        //! Set the slot's name.
        virtual void SetName(const AZStd::string&) = 0;

        //! Get and set the keys used for slot name translation.
        virtual TranslationKeyedString GetTranslationKeyedName() const = 0;
        virtual void SetTranslationKeyedName(const TranslationKeyedString&) = 0;

        //! Get the tooltip for the slot.
        virtual const AZStd::string& GetTooltip() const = 0;
        //! Set the tooltip this slot should display.
        virtual void SetTooltip(const AZStd::string&) = 0;

        //! Get and set the keys used for slot tooltip translation.
        virtual TranslationKeyedString GetTranslationKeyedTooltip() const = 0;
        virtual void SetTranslationKeyedTooltip(const TranslationKeyedString&) = 0;

        //! Get the group of the slot
        virtual SlotGroup GetSlotGroup() const = 0;

        //! Get the type of the slot.
        virtual SlotType GetSlotType() const = 0;

        //! Get the connection type of the slot is in.
        virtual ConnectionType GetConnectionType() const = 0;

        //! Returns whether or not this slot will accept a connection to the passed in slot.
        virtual bool CanAcceptConnection(const Endpoint& endpoint) = 0;

        //! Returns the connection to be used when trying to create a connection from this object.
        virtual AZ::EntityId CreateConnection() const = 0;

        //! Returns the connection to be used when trying to create a connection from this object.
        virtual AZ::EntityId CreateConnectionWithEndpoint(const Endpoint& endpoint) const = 0;

        //! Displays the proposed connection on the slot
        virtual void DisplayProposedConnection(const AZ::EntityId& connectionId, const Endpoint& endpoint) = 0;

        //! Restores the connection display to the previous state.
        virtual void RemoveProposedConnection(const AZ::EntityId& connectionId, const Endpoint& endpoint) = 0;

        //! Adds the given connection to the slot
        virtual void AddConnectionId(const AZ::EntityId& connectionId, const Endpoint& endpoint) = 0;

        //! Remove the specified connection from the slot.
        virtual void RemoveConnectionId(const AZ::EntityId& connectionId, const Endpoint& endpoint) = 0;
        
        //! Gets the UserData on the slot.
        virtual AZStd::any* GetUserData() = 0;

        //! Returns whether or not the slot has any connections
        virtual bool HasConnections() const = 0;

        //! Returns the last connection connected to the slot
        //! Returns an invalid EntityId if the slot has no connections
        virtual AZ::EntityId GetLastConnection() const = 0;

        //! Returns the list of connections connected to this slot
        virtual AZStd::vector<AZ::EntityId> GetConnections() const = 0;

        //! Sets the specified display state onto all of the connected connections.
        virtual void SetConnectionDisplayState(ConnectionDisplayState displayState) = 0;

        //! Clears all of the connections currently attached to this slot.
        virtual void ClearConnections() = 0;
    };

    using SlotRequestBus = AZ::EBus<SlotRequests>;

    class SlotUIRequests
        : public AZ::EBusTraits
    {
    public:
        //! BusId is the entity id of the slot object.
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;
        using BusIdType = AZ::EntityId;

        virtual QPointF GetConnectionPoint() const = 0;
    };

    using SlotUIRequestBus = AZ::EBus<SlotUIRequests>;

    //! SlotNotifications
    //! Notifications that indicate changes to a slot's state.
    class SlotNotifications
        : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;
        using BusIdType = AZ::EntityId;

        //! When the name of the slot changes, the new name is signalled.
        virtual void OnNameChanged(const TranslationKeyedString&) {}
        //! When the tooltip of the slot changes, the new tooltip value is emitted.
        virtual void OnTooltipChanged(const TranslationKeyedString&) {}

        virtual void OnRegisteredToNode(const AZ::EntityId&) {}
        
        //! When the slot configuration changes, then this event is signalled.
        virtual void OnSlotConfigChanged() {}

        //! When the slot becomes an end of a new connection, it provides a notification of the connection and the
        //! other slot, in that order.
        virtual void OnConnectedTo(const AZ::EntityId& connectionId, const Endpoint& endpoint) { (void)connectionId; (void)endpoint; }

        //! When the slot ceases to be an end of a connection, it provides a notification of the connection and the
        //! other slot, in that order.
        virtual void OnDisconnectedFrom(const AZ::EntityId& connectionId, const Endpoint& endpoint) { (void)connectionId; (void)endpoint; }
    };

    using SlotNotificationBus = AZ::EBus<SlotNotifications>;

    class SlotLayoutRequests
        : public AZ::EBusTraits
    {
    public:
        // Id here is the ID of the node that contains the SlotLayout.
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;
        using BusIdType = AZ::EntityId;

        virtual void SetDividersEnabled(bool enabled) = 0;
        virtual void ConfigureSlotGroup(SlotGroup group, SlotGroupConfiguration configuration) = 0;

        virtual void SetSlotGroupVisible(SlotGroup group, bool visible) = 0;

        virtual void ClearSlotGroup(SlotGroup group) = 0;
    };

    using SlotLayoutRequestBus = AZ::EBus<SlotLayoutRequests>;
}