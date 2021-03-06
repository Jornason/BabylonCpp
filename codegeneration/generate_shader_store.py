import os

def rreplace(s, old, new, occurrence = 1):
    """
    Replaces the last occurence(s) of an expression in a string.
    """
    return new.join(s.rsplit(old, occurrence))

def createDir(path):
    """
    Creates the specified directory if not exists.
    """
    import errno
    # create dir
    try:
        os.makedirs(path)
    except OSError as exception:
        if exception.errno != errno.EEXIST:
            raise

def prepareFilename(name):
    """
    Converts upper case characters to lower case and adds '_' in front.
    """
    import re
    s = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s).lower()

def shaderFilenameToVariableName(filename, isInclude = False):
    """
    Determines the variable name from the filename.
    """
    split = filename.split(".")[:-1] 
    if not isInclude:
        split += ["shader"]
    for i in range(1, len(split)):
        split[i] = "%s%s" % (split[i][0].upper(), split[i][1:])
    varName = ''.join(split)
    if varName.endswith("FragmentShader"):
        varName = varName.replace("FragmentShader", "PixelShader")
    return varName

def processShaderFile(shaderPath, outputDir, definePath="BABYLON_SHADERS",
                      isInclude = False):
    """
    Puts the shader file content into a header file.
    """
    import codecs
    # name case fixes in output path
    if "triPlanar" in outputDir:
        outputDir = outputDir.replace("triPlanar", "triplanar")
    # read shader file contents
    shaderVarName = os.path.basename(shaderPath)
    if "triplanar" in outputDir:
        shaderVarName = shaderVarName.replace("triplanar", "triPlanar")
    shaderFilename = prepareFilename(shaderVarName)
    lines = [line.rstrip('\n').rstrip('\r').decode("utf-8-sig") \
                                                for line in open(shaderPath)]
    # generate header file content
    outputFileName = "%s.h" % shaderFilename.replace(".", "_")
    defineName = "%s_%s" % (definePath,
                            outputFileName.replace(".", "_").upper())
    if isInclude:
        defineName = defineName.replace("BABYLON_SHADERS",
                                        "BABYLON_SHADERS_SHADERS_INCLUDE")
    varName = shaderFilenameToVariableName(shaderVarName, isInclude)
    if isInclude:
        varName = shaderFilenameToVariableName(os.path.basename(shaderPath),
                                               isInclude)
    eol = '\n'
    output = "#ifndef %s%s" % (defineName, eol)
    output += "#define %s%s%s" % (defineName, eol, eol)
    output += "namespace BABYLON {%s%s" % (eol, eol)
    # process first line
    output += "extern const char* %s;%s%s" % (varName, eol, eol)
    output += "const char* %s%s  = " % (varName, eol)
    if lines[0] == "precision highp float;":
        output += "\"%s\\n\"%s" % ("#ifdef GL_ES", eol)
        output += "    \"%s\\n\"%s" % (lines[0], eol)
        output += "    \"%s\\n\"%s" % ("#endif", eol)
    else:
        output += "\"%s\\n\"%s" % (lines[0], eol)
    # process remainder of the shader file
    for i in range(1, len(lines)-1):
        lines[i] = lines[i].replace('\"', "\\\"")
        lines[i] = lines[i].replace('\t', " " * 2)
        while lines[i].startswith(" " * 4):
            lines[i] = lines[i].replace(" " * 4, " " * 2)
        if lines[i] == "precision highp float;":
            output += "    \"%s\\n\"%s" % ("#ifdef GL_ES", eol)
            output += "    \"%s\\n\"%s" % (lines[i], eol)
            output += "    \"%s\\n\"%s" % ("#endif", eol)
        else:
            output += "    \"%s\\n\"%s" % (lines[i], eol)
    output += "    \"%s\\n\";%s%s" % (lines[-1], eol, eol)
    output += "} // end of namespace BABYLON%s%s" % (eol, eol)
    output += "#endif // end of %s%s" % (defineName, eol)
    # write header file content to file
    outputFileLocation = os.path.join(outputDir, outputFileName)
    with codecs.open(outputFileLocation, "w", "utf-8-sig") as file:
        file.write(output)
    return outputFileLocation

def generateShadersStore(shaderFiles, outputDir,
                         outputFileName = "effect_shaders_store.cpp"):
    """
    Generates the "effect_shaders_store.cpp" file containing the shaders map.
    """
    import codecs
    ### generate header file ###
    eol = '\n'
    headerFilename = outputFileName.replace(".cpp", ".h")
    output = "#ifndef BABYLON_MATERIALS_EFFECT_SHADERS_STORE_H%s" % eol
    output += "#define BABYLON_MATERIALS_EFFECT_SHADERS_STORE_H%s%s" % (eol,eol)
    output += "#include <babylon/babylon_global.h>%s%s" % (eol, eol)
    output += "namespace BABYLON {%s%s" % (eol, eol)
    output += "struct BABYLON_SHARED_EXPORT EffectShadersStore {%s" % eol
    output += "  static std::unordered_map<std::string, const char*> "
    output += "Shaders;%s" % eol
    output += "}; // end of struct EffectShadersStore%s%s" % (eol, eol)
    output += "} // end of namespace BABYLON%s%s#endif " % (eol, eol)
    output += "// end of BABYLON_MATERIALS_EFFECT_SHADERS_STORE_H%s" % eol
    # write output to file
    hdir = rreplace(outputDir, "src", os.path.join("include", "babylon"), 1)
    outputFileLocation = os.path.join(hdir, headerFilename)
    with codecs.open(outputFileLocation, "w", "utf-8-sig") as file:
        file.write(output)
    ### generate source file containg the shader map ###
    output = "#include <babylon/materials/%s>%s%s" % \
                                (outputFileName.replace(".cpp", ".h"), eol, eol)
    shaderNames = []
    for shaderFile in shaderFiles:
        shaderFilename = os.path.basename(shaderFile)
        shaderNames.append(shaderFilenameToVariableName(shaderFilename))
        shaderFilename = prepareFilename(shaderFilename)
        output += "#include <babylon/shaders/%s>%s" % ("%s.h" % \
                                        shaderFilename.replace(".", "_"), eol)
    output += "%snamespace BABYLON {%s%s" % (eol, eol, eol)
    output += "std::unordered_map<std::string, const char*> "
    output += "EffectShadersStore::Shaders = {%s" % eol
    for shaderName in shaderNames:
        output += "  {\"%s\", %s},%s" % (shaderName, shaderName, eol)
    output = "%s%s};%s%s" % (output[:-2], eol, eol, eol)
    output += "} // end of namespace BABYLON%s" % eol
    # write output to file
    outputFileLocation = os.path.join(outputDir, outputFileName)
    with codecs.open(outputFileLocation, "w", "utf-8-sig") as file:
        file.write(output)
    return outputFileLocation

def generateIncludesShadersStore(shaderFiles, outputDir,
                        outputFileName = "effect_includes_shaders_store.cpp"):
    """
    Generates the "effect_includes_shaders_store.cpp" file containing the
    shaders includes map.
    """
    import codecs
    ### generate header file ###
    eol = '\n'
    headerFilename = outputFileName.replace(".cpp", ".h")
    output = "#ifndef BABYLON_MATERIALS_EFFECT_INCLUDES_SHADERS_STORE_H%s" % eol
    output += "#define BABYLON_MATERIALS_EFFECT_INCLUDES_SHADERS_STORE_H%s%s"  \
                                                                    % (eol,eol)
    output += "#include <babylon/babylon_global.h>%s%s" % (eol, eol)
    output += "namespace BABYLON {%s%s" % (eol, eol)
    output += "struct BABYLON_SHARED_EXPORT EffectIncludesShadersStore {%s"% eol
    output += "  static std::unordered_map<std::string, const char*> "
    output += "Shaders;%s" % eol
    output += "}; // end of struct EffectIncludesShadersStore%s%s" % (eol, eol)
    output += "} // end of namespace BABYLON%s%s#endif " % (eol, eol)
    output += "// end of BABYLON_MATERIALS_EFFECT_INCLUDES_SHADERS_STORE_H%s"  \
                                                                        % eol
    # write output to file
    hdir = rreplace(outputDir, "src", os.path.join("include", "babylon"), 1)
    outputFileLocation = os.path.join(hdir, headerFilename)
    with codecs.open(outputFileLocation, "w", "utf-8-sig") as file:
        file.write(output)
    ### generate source file containg the shader map ###
    output = "#include <babylon/materials/%s>%s%s" % (headerFilename, eol, eol)
    shaderNames = []
    for shaderFile in shaderFiles:
        shaderFilename = os.path.basename(shaderFile)
        shaderNames.append(shaderFilenameToVariableName(shaderFilename, True))
        shaderFilename = prepareFilename(shaderFilename)
        output += "#include <babylon/shaders/shadersinclude/%s>%s" % ("%s.h" % \
                                        shaderFilename.replace(".", "_"), eol)
    output += "%snamespace BABYLON {%s%s" % (eol, eol, eol)
    output += "std::unordered_map<std::string, const char*> "
    output += "EffectIncludesShadersStore::Shaders  %s = {" % eol
    for shaderName in shaderNames:
        output += "{\"%s\", %s},%s   " % (shaderName, shaderName, eol)
    output = "%s%s};%s%s" % (output[:-5], eol, eol, eol)
    output += "} // end of namespace BABYLON%s" % eol
    # write output to file
    outputFileLocation = os.path.join(outputDir, outputFileName)
    with codecs.open(outputFileLocation, "w", "utf-8-sig") as file:
        file.write(output)
    return outputFileLocation

def sortDictionaryByKey(directory):
    """
    Sorts a dictonary by key in ascending order.
    """
    import collections
    return collections.OrderedDict(sorted(directory.items()))

def getFilesnamesInDir(directory, shaderExtension = ".fx"):
    """
    Returns all the files in the specified directory.
    """
    files = {}
    for f in os.listdir(directory):
        fileLocation =  os.path.join(directory, f)
        if os.path.isfile(fileLocation) and f.endswith(shaderExtension):
            files[f] = fileLocation
    return files

def getDirectoriesInDir(directory):
    """
    Returns all the directories in the specified directory.
    """
    directories = {}
    for d in os.listdir(directory):
        path =  os.path.join(directory, d)
        if os.path.isdir(path):
            directories[d] = path
    return directories

def generateShadersStores(shadersInputDir, shadersOutputDir):
    """
    Generates the shaders stores files from the Babylon.js shaders folder.
    """
    #### process the shader files ####
    import time
    # - standard shaders
    shadersInputDir = os.path.join(inputDir, "src", "Shaders")
    shadersOutputDir = os.path.join(outputDir, "..", "BabylonCpp", "include",
                                    "babylon", "shaders")
    shadersStoreDir = os.path.join(outputDir, "..", "BabylonCpp", "src",
                                   "materials")
    shaderFiles = getFilesnamesInDir(shadersInputDir)
    dlc = len(shaderFiles)
    print "Found %d standard shaders" % dlc
    # - sort shader files by filename
    shaderFiles = sortDictionaryByKey(shaderFiles)
    # - process shader files
    indent = ' ' * 6
    tic = time.time()
    for shaderFileName, shaderPath in shaderFiles.items():
        processShaderFile(shaderPath, shadersOutputDir)
        print "%s|-Processed shader file: %s" % (indent, shaderFileName)
    toc = time.time()
    print "Processed %d shader files in %ss" % (dlc, (toc - tic))
    # - generate the shader store
    generateShadersStore(shaderFiles, shadersStoreDir)
    print "Generated shader store"
    ### process the includes shader files ###
    shadersInputDir = os.path.join(shadersInputDir, "ShadersInclude")
    shadersOutputDir = os.path.join(shadersOutputDir, "shadersinclude")
    createDir(shadersOutputDir)
    shaderFiles = getFilesnamesInDir(shadersInputDir)
    slc = len(shaderFiles)
    print "Found %d standard shaders includes" % slc
    # - sort shader files by filename
    shaderFiles = sortDictionaryByKey(shaderFiles)
    tic = time.time()
    for shaderFileName, shaderPath in shaderFiles.items():
        processShaderFile(shaderPath, shadersOutputDir, "BABYLON_SHADERS", True)
        print "%s|-Processed shader incude file: %s" % (indent, shaderFileName)
    toc = time.time()
    print "Processed %d shader include files in %ss" % (slc, (toc - tic))
    # generate the includes shader store
    generateIncludesShadersStore(shaderFiles, shadersStoreDir)
    print "Generated shader include store"

def generateProceduralTexturesShaderHeaders(inputDir, outputDir):
    """
    Generates procedural textures library header files.
    """
    import time
    proceduralTexturesInputDir = os.path.join(inputDir,
                                        "proceduralTexturesLibrary", "src")
    proceduralTexturesOuputDir = os.path.join(outputDir, "..",
                                        "ProceduralTexturesLibrary", "include",
                                        "babylon", "proceduraltextureslibrary")
    definePathPrefix = "BABYLON_PROCEDURAL_TEXTURES_LIBRARY"
    shaderInputFiles = {}
    shaderOutputFiles = {}
    ptlc = 0
    if os.path.isdir(proceduralTexturesInputDir):
        materialsLibraryDirs = getDirectoriesInDir(proceduralTexturesInputDir)
        for shaderName, path in materialsLibraryDirs.items():
            proceduralTextures = getFilesnamesInDir(path)
            shaderInputFiles.update(proceduralTextures)
            for shaderFileName, shaderPath in proceduralTextures.items():
                shaderOutputFiles[shaderFileName] = \
                            os.path.join(proceduralTexturesOuputDir, shaderName)
            ptlc += len(proceduralTextures)
    print "Found %d shaders in procedural textures library" % ptlc
    # - process shader files
    indent = ' ' * 6
    tic = time.time()
    for shaderFileName, shaderInputPath in shaderInputFiles.items():
        definePath = "%s_%s" % (definePathPrefix, os.path.basename(
                                shaderOutputFiles[shaderFileName]).upper())
        processShaderFile(shaderInputPath, shaderOutputFiles[shaderFileName],
                          definePath)
        print "%s|-Processed shader file: %s" % (indent, shaderFileName)
    toc = time.time()
    print "Processed %d shader files in %ss" % (ptlc, (toc - tic))

def generateMaterialsLibraryShaderHeaders(inputDir, outputDir):
    """
    Generates materials library header files.
    """
    import time
    materialsLibraryInputDir = os.path.join(inputDir,
                                        "materialsLibrary", "src")
    materialsLibraryOuputDir = os.path.join(outputDir, "..",
                                        "MaterialsLibrary", "include",
                                        "babylon", "materialslibrary")
    definePathPrefix = "BABYLON_MATERIALS_LIBRARY"
    shaderInputFiles = {}
    shaderOutputFiles = {}
    mlc = 0
    if os.path.isdir(materialsLibraryInputDir):
        materialsLibraryDirs = getDirectoriesInDir(materialsLibraryInputDir)
        for shaderName, path in materialsLibraryDirs.items():
            materials = getFilesnamesInDir(path)
            shaderInputFiles.update(materials)
            for shaderFileName, shaderPath in materials.items():
                shaderOutputFiles[shaderFileName] = \
                            os.path.join(materialsLibraryOuputDir, shaderName)
            mlc += len(materials)
    print "Found %d shaders in materials library" % mlc
    # - process shader files
    indent = ' ' * 6
    tic = time.time()
    for shaderFileName, shaderInputPath in shaderInputFiles.items():
        definePath = "%s_%s" % (definePathPrefix, os.path.basename(
                                shaderOutputFiles[shaderFileName]).upper())
        processShaderFile(shaderInputPath, shaderOutputFiles[shaderFileName],
                          definePath)
        print "%s|-Processed shader file: %s" % (indent, shaderFileName)
    toc = time.time()
    print "Processed %d shader files in %ss" % (mlc, (toc - tic))

if __name__ == "__main__":
    from optparse import OptionParser

    parser = OptionParser(usage="usage: %prog [options] inputDir outputDir",
                          version="%prog 1.0")
    parser.add_option("-i", "--inputDir", action="store", dest="inputDir",
                      type="string", help="Babylon.js root path")
    parser.add_option("-o", "--outputDir", action="store",
                      dest="outputDir", type="string",
                      help="BabylonCpp root path")
    (options, args) = parser.parse_args()

    # set default values when input is missing
    if not options.inputDir:
        options.inputDir = os.path.join(os.getcwd(), "..", "..", "..",
                                        "Projects", "Babylon.js-2.5")
        args += [options.inputDir]
    if not options.outputDir:
        options.outputDir = os.path.join(os.getcwd(), "..", "src", "BabylonCpp")
        args += [options.outputDir]

    # variables needed by the generator
    inputDir = None
    outputDir = None

    # check arguments
    if len(args) != 2:
        if not options.inputDir and not options.outputDir:
            parser.error("Incorrect number of arguments")

    # check if the shaders input folder exists
    if options.inputDir and os.path.isdir(options.inputDir):
        inputDir = options.inputDir
    else:
        parser.error("Incorrect input directory")

    # check if the shaders output folder exists
    if options.outputDir and os.path.isdir(options.outputDir):
        tmp = os.path.join(options.outputDir, "src", "materials")
        if not os.path.isdir(tmp):
            parser.error("Incorrect output directory")
        else:
            outputDir = options.outputDir
    else:
        parser.error("Incorrect output directory")

    # generate the shader store
    generateShadersStores(inputDir, outputDir)
    #generateProceduralTexturesShaderHeaders(inputDir, outputDir)
    #generateMaterialsLibraryShaderHeaders(inputDir, outputDir)
