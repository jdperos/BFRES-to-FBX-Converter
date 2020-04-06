import sys
import os
import shutil

initialWD = os.getcwd()

isDebug = True
writeTextures = ""
if len(sys.argv) > 1:
	i = 1
	while i < len(sys.argv):
		if sys.argv[i] == "-t":
			writeTextures = " -t"
		elif sys.argv[i] == "-r":
			isDebug = False
		else:
			raise Exception("Invalid argmument! Use '-r' for release and '-t' to write textures.")
		i += 1
configType = "Debug" if isDebug else "Release"
	
inDir = os.path.join(initialWD, "In\\")
outDir = os.path.join(initialWD, "Out\\")

importerFP = os.path.join(initialWD, "Importer", configType, "BFRESImporter.exe")
exporterFP = os.path.join(initialWD, "Exporter", configType, "FBXExporter.exe")

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
		importerCommand = "\"" + importerFP + "\" \"" + os.path.join(inDir,sbfresFile) + "\" \"" + fileGroupSubPath + "/\"" 
		print("Running Importer:"  + importerCommand )
		os.system( "\"" + importerCommand + "\"")
		

		# If it's not a texture Bfres, run the exporter
		inputXMLPath = os.path.join(fileGroupSubPath,fileNameNoExt + ".xml")
		if not sbfresFile.endswith(".Tex1.sbfres"):
			if not sbfresFile.endswith(".Tex2.sbfres"):
				print("Input XML Path = " + inputXMLPath)
				outputFBXPath = fileGroupSubPath + "/"
				print("Export Path" + outputFBXPath)
				exporterCommand = "\"" + exporterFP + "\" \"" + inputXMLPath + "\" \"" + outputFBXPath + "\"" + writeTextures
				os.system( "\"" + exporterCommand + "\"" )
				# Throw an error if an FBX is not written
				outputFBXFilename = os.path.splitext(os.path.basename(inputXMLPath))[0]
				if not os.path.exists(os.path.join(outputFBXPath, outputFBXFilename + ".fbx")):
					with open('log.csv', 'a+') as logFile:
						logFile.write("\n" + outputFBXFilename + ";-1;Fbx file failed to write.")

		
		os.system("xcopy /y \"" + inputXMLPath + "\" \""+ os.path.join(fileGroupSubPath,"XMLDumps","\""))
		os.system("del /q \"" + inputXMLPath + "\"")