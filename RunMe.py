import os
import shutil

initialWD = os.getcwd()

inDir = os.path.join(initialWD, "In\\")
outDir = os.path.join(initialWD, "Out\\")
importerFP = os.path.join(initialWD, "Importer", "BFRES Importer.exe")
exporterFP = os.path.join(initialWD, "Exporter", "BFRES to FBX Converter.exe")

for sbfresFile in sorted(os.listdir(inDir)):
	if sbfresFile.endswith(".sbfres"): 
		fileNameNoExt = sbfresFile[0:len(sbfresFile) - len(".sbfres")]
		# Set file group name
		print("SBfres file: " + sbfresFile)
		if sbfresFile.endswith("_Animation.sbfres"): 
			fileGroupName = sbfresFile[0:len(sbfresFile) - len("_Animation.sbfres")]
		elif sbfresFile.endswith(".Tex1.sbfres"): 
			fileGroupName = sbfresFile[0:len(sbfresFile) - len(".Tex1.sbfres")]
		elif sbfresFile.endswith(".Tex2.sbfres"): 
			fileGroupName = sbfresFile[0:len(sbfresFile) - len(".Tex2.sbfres")]
		else:
			fileGroupName = sbfresFile[0:len(sbfresFile) - len(".sbfres")]
		print("Filegroup Name: " + fileGroupName)
		
		# Create filegroup subdirectory
		fileGroupSubPath = os.path.join(outDir, fileGroupName)
		if not os.path.exists(fileGroupSubPath):
			print("Creating subdirectory for filegroup: " + fileGroupSubPath)
			os.makedirs(fileGroupSubPath)
		else:
			print("Subdirectory for filegroup exists already")
			
		# Run Importer
		print("\"" + importerFP + "\" " + inDir + sbfresFile + " " + fileGroupSubPath + "\\" )
		os.system("\"" + importerFP + "\" " + inDir + sbfresFile + " " + fileGroupSubPath + "\\" )
		
		if not sbfresFile.endswith(".Tex1.sbfres"):
			if not sbfresFile.endswith(".Tex2.sbfres"):
				print("fileGroupSubPath=" + fileGroupSubPath)
				inputXMLPath = fileGroupSubPath + "\\" + fileNameNoExt + ".xml"
				print(inputXMLPath)
				outputFBXPath = fileGroupSubPath + "\\"
				print(outputFBXPath)
				os.system("\"" + exporterFP + "\" " + inputXMLPath + " " + outputFBXPath)
		