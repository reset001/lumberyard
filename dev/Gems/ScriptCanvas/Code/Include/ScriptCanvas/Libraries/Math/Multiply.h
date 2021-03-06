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

#include <Libraries/Core/BinaryOperator.h>
#include <Libraries/Math/ArithmeticFunctions.h>

namespace ScriptCanvas
{
    namespace Nodes
    {
        namespace Math
        {
            class Multiply
                : public ArithmeticExpression
            {
            public:
                AZ_COMPONENT(Multiply, "{1BC9A5A9-9BF3-4DA7-A8F7-911254AEB243}", ArithmeticExpression);

                static void Reflect(AZ::ReflectContext* reflection)
                {
                    if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
                    {
                        serializeContext->Class<Multiply, ArithmeticExpression>()
                            ->Version(0)
                            ;

                        if (AZ::EditContext* editContext = serializeContext->GetEditContext())
                        {
                            editContext->Class<Multiply>("Multiply", "Multiply")
                                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                                    ->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/ScriptCanvas/Placeholder.png")
                                    ->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly)
                                ;
                        }
                    }
                }
                
                void Visit(NodeVisitor& visitor) const override
                {
                    visitor.Visit(*this);
                }

            protected:
                Datum Evaluate(const Datum& lhs, const Datum& rhs) override
                {
                    return Datum::CreateInitializedCopy(*lhs.GetAs<Data::NumberType>() * *rhs.GetAs<Data::NumberType>());
                }
            };

#if defined(EXPRESSION_TEMPLATES_ENABLED)
            class Multiply
                : public BinaryOperatorGeneric<Multiply, ArithmeticOperator<OperatorType::Mul>>
            {
            public:
                using BaseType = BinaryOperatorGeneric<Multiply, ArithmeticOperator<OperatorType::Mul>>;
                AZ_COMPONENT(Multiply, "{39F9C2B4-B5A0-4DE1-921B-313C59FD4596}", BaseType);

                static const char* GetOperatorName() { return "Multiply"; }
                static const char* GetOperatorDesc() { return "Perform multiplication between two numbers"; }
                static const char* GetIconPath() { return "Editor/Icons/ScriptCanvas/Multiply.png"; }

                void Visit(NodeVisitor& visitor) const override { visitor.Visit(*this); }

            };
#endif // #if defined(EXPRESSION_TEMPLATES_ENABLED)
        }
    }
}

