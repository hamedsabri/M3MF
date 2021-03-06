/*++

Copyright (C) 2020 3MF Consortium

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Abstract:
NMR_ModelReaderNode_BeamLattice1702_Ball.h covers the official 3MF beamlattice extension.

--*/

#ifndef __NMR_MODELREADERNODE_BEAMLATTICE1702_BALL
#define __NMR_MODELREADERNODE_BEAMLATTICE1702_BALL

#include "Model/Reader/NMR_ModelReaderNode.h"
#include "Model/Classes/NMR_ModelComponent.h"
#include "Model/Classes/NMR_ModelComponentsObject.h"
#include "Model/Classes/NMR_ModelObject.h"

namespace NMR {
	eModelBeamLatticeBallMode stringToBallMode(const nfChar * ballModeStr);

	class CModelReaderNode_BeamLattice1702_Ball : public CModelReaderNode {
	private:
		nfInt32 m_nIndex;
		nfBool m_bHasRadius;
		nfDouble m_dRadius;
		nfBool m_bHasTag;
		nfInt32 m_nTag;
	protected:
		virtual void OnAttribute(_In_z_ const nfChar * pAttributeName, _In_z_ const nfChar * pAttributeValue);
		virtual void OnNSAttribute(_In_z_ const nfChar * pAttributeName, _In_z_ const nfChar * pAttributeValue, _In_z_ const nfChar * pNameSpace);
	public:
		CModelReaderNode_BeamLattice1702_Ball() = delete;
		CModelReaderNode_BeamLattice1702_Ball(_In_ CModel * pModel, _In_ PModelWarnings pWarnings);

		virtual void parseXML(_In_ CXmlReader * pXMLReader);

		void retrieveIndex(_Out_ nfInt32 & nIndex, nfInt32 nNodeCount);
		void retrieveRadius(_Out_ nfBool & bHasRadius, _Out_ nfDouble & dRadius);
		void retrieveTag(_Out_ nfBool & bHasTag, _Out_ nfInt32 & tag);
	};

	typedef std::shared_ptr <CModelReaderNode_BeamLattice1702_Ball> PModelReaderNode_BeamLattice1702_Ball;
}

#endif // __NMR_MODELREADERNODE_BEAMLATTICE1702_BALL