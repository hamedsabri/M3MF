/*++

Copyright (C) 2019 3MF Consortium

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

NMR_ModelReaderNode100_Color.cpp implements the Model Reader Color Node Class.

--*/

#include "Model/Reader/v100/NMR_ModelReaderNode100_Composite.h"

#include "Model/Classes/NMR_ModelConstants.h"
#include "Model/Classes/NMR_ModelMeshObject.h"

#include "Common/NMR_StringUtils.h"
#include "Common/NMR_Exception.h"
#include "Common/NMR_Exception_Windows.h"

namespace NMR {

	CModelReaderNode100_Composite::CModelReaderNode100_Composite(_In_ CModel * pModel, _In_ PModelWarnings pWarnings,
		_In_z_ const std::vector<ModelPropertyID>& baseMaterialPropertyIds)
		: CModelReaderNode(pWarnings), m_vctBaseMaterialPropertyIds(baseMaterialPropertyIds)
	{
		m_pModel = pModel;
	}

	void CModelReaderNode100_Composite::parseXML(_In_ CXmlReader * pXMLReader)
	{
		// Parse name
		parseName(pXMLReader);

		// Parse attribute
		parseAttributes(pXMLReader);

		// Parse Content
		parseContent(pXMLReader);

	}

	PModelComposite CModelReaderNode100_Composite::getComposite()
	{
		return m_pModelComposite;
	}

	void CModelReaderNode100_Composite::OnAttribute(_In_z_ const nfChar * pAttributeName, _In_z_ const nfChar * pAttributeValue)
	{
		__NMRASSERT(pAttributeName);
		__NMRASSERT(pAttributeValue);

		if (strcmp(pAttributeName, XML_3MF_ATTRIBUTE_COMPOSITE_VALUES) == 0) {
			m_pModelComposite = std::make_shared<CModelComposite>();
			std::vector<nfDouble> vctMixingRatios = fnVctType_fromString<nfDouble>(pAttributeValue);
			for (size_t i = 0; i < m_vctBaseMaterialPropertyIds.size(); i++) {
				MODELCOMPOSITECONSTITUENT constituent;
				constituent.m_nPropertyID = m_vctBaseMaterialPropertyIds[i];
				if (i < vctMixingRatios.size()) {
					constituent.m_dMixingRatio = vctMixingRatios[i];
				}
				else {
					constituent.m_dMixingRatio = 0;
				}
				m_pModelComposite->push_back(constituent);
			}
			if (vctMixingRatios.size() < m_vctBaseMaterialPropertyIds.size())
				m_pWarnings->addException(CNMRException(NMR_ERROR_MIXINGRATIO_MISSING), mrwInvalidOptionalValue);
			else if (vctMixingRatios.size() > m_vctBaseMaterialPropertyIds.size()) {
				m_pWarnings->addException(CNMRException(NMR_ERROR_MIXINGRATIO_TOOMANY), mrwInvalidOptionalValue);
			}
		} else {
			m_pWarnings->addException(CNMRException(NMR_ERROR_NAMESPACE_INVALID_ATTRIBUTE), mrwInvalidOptionalValue);
		}
	}

}
