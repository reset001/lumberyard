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

#pragma once

// include the required files
#include "EMotionFXConfig.h"
#include "ConstraintTransform.h"

#include <MCore/Source/Matrix4.h>
#include <AzCore/Math/Vector2.h>


namespace EMotionFX
{
    /**
     * The rotation angle constraint.
     * This constraint works on a transform and limits the rotation to be within a given range defined by minimum and maximum swing and twist angles in degrees.
     */
    class EMFX_API ConstraintTransformRotationAngles
        : public ConstraintTransform
    {
        MCORE_MEMORYOBJECTCATEGORY(ConstraintTransformRotationAngles, EMFX_DEFAULT_ALIGNMENT, EMFX_MEMCATEGORY_CONSTRAINTS);

    public:
        enum
        {
            TYPE_ID = 0x00000001
        };

        enum EAxis
        {
            AXIS_X = 0,
            AXIS_Y = 1,
            AXIS_Z = 2
        };

        ConstraintTransformRotationAngles();
        ~ConstraintTransformRotationAngles() override = default;

        uint32 GetType() const override;
        const char* GetTypeString() const override;
        void Execute() override;

        void DebugDraw(const MCore::Matrix& offset, uint32 color, float radius) const;

        void SetMinRotationAngles(const AZ::Vector2& minSwingDegrees);
        void SetMaxRotationAngles(const AZ::Vector2& maxSwingDegrees);
        void SetMinTwistAngle(float minAngleDegrees);
        void SetMaxTwistAngle(float maxAngleDegrees);
        void SetTwistAxis(EAxis axisIndex);

        AZ::Vector2 GetMinRotationAnglesDegrees() const;        // All returned angles are in degrees.
        AZ::Vector2 GetMaxRotationAnglesDegrees() const;
        AZ::Vector2 GetMinRotationAnglesRadians() const;        // All returned angles are in radians.
        AZ::Vector2 GetMaxRotationAnglesRadians() const;
        float GetMinTwistAngle() const;
        float GetMaxTwistAngle() const;
        EAxis GetTwistAxis() const;

    protected:
        AZ::Vector2     mMinRotationAngles;         ///< The minimum rotation angles, actually the precalculated sin(halfAngleRadians).
        AZ::Vector2     mMaxRotationAngles;         ///< The maximum rotation angles, actually the precalculated sin(halfAngleRadians).
        float           mMinTwist;                  ///< The minimum twist angle, actually the precalculated sin(halfAngleRadians).
        float           mMaxTwist;                  ///< The maximum twist angle, actually the precalculated sin(halfAngleRadians).
        EAxis           mTwistAxis;                 ///< The twist axis index, which has to be either 0, 1 or 2 (default=AXIS_X, which equals 0).

        void DrawSphericalLine(const AZ::Vector2& start, const AZ::Vector2& end, uint32 numSteps, uint32 color, float radius, const MCore::Matrix& offset) const;
        AZ::Vector3 GetSphericalPos(float x, float y) const;
    };
}   // namespace EMotionFX
