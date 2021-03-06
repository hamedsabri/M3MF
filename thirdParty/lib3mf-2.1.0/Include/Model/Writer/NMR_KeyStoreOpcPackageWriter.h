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

NMR_KeyStoreOpcPackageWriter.h defines an OPC Package writer in a portable way.

--*/

#ifndef NMR_KEYSTOREOPCPACKAGEWRITER
#define NMR_KEYSTOREOPCPACKAGEWRITER

#include <memory>

#include "Common/OPC/NMR_IOpcPackageWriter.h"
#include "Common/Platform/NMR_ExportStream.h"

namespace NMR {

	class CModelContext;
	class CKeyStoreResourceData;
	class CKeyStoreAccessRight;
	class CXmlWriter;

	using PKeyStoreResourceData = std::shared_ptr<CKeyStoreResourceData>;
	using PKeyStoreAccessRight = std::shared_ptr<CKeyStoreAccessRight>;

	class CKeyStoreOpcPackageWriter : public IOpcPackageWriter {
	private:
		bool pathIsEncrypted(_In_ std::string sPath);
	protected:
		CModelContext const & m_pContext;
		PIOpcPackageWriter m_pPackageWriter;

		void writeKeyStoreStream(_In_ CXmlWriter * pXMLWriter);
		void refreshAllResourceDataGroups();
		POpcPackagePart wrapPartStream(PKeyStoreResourceData rd, POpcPackagePart part);
		void refreshResourceDataTag(PKeyStoreResourceData rd);
		void refreshAccessRight(PKeyStoreAccessRight ar, std::vector<nfByte> const & key);
	public:
		CKeyStoreOpcPackageWriter(
			_In_ PExportStream pImportStream, 
			_In_ CModelContext const & context);

		POpcPackagePart addPart(_In_ std::string sPath) override;
		void close() override;
		void addContentType(std::string sExtension, std::string sContentType) override;
		void addContentType(_In_ POpcPackagePart pOpcPackagePart, _In_ std::string sContentType) override;
		POpcPackageRelationship addRootRelationship(std::string sType, COpcPackagePart * pTargetPart) override;
		POpcPackageRelationship addPartRelationship(_In_ POpcPackagePart pOpcPackagePart, _In_ std::string sType, _In_ COpcPackagePart * pTargetPart) override;
		std::list<POpcPackageRelationship> addWriterSpecificRelationships(_In_ POpcPackagePart pOpcPackagePart, _In_ COpcPackagePart* pTargetPart) override;
	};

	using PKeyStoreOpcPackageWriter = std::shared_ptr<CKeyStoreOpcPackageWriter>;

}

#endif // !NMR_KEYSTOREOPCPACKAGEWRITER
