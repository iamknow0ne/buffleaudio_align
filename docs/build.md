# Build Notes

The current project now has a CMake build path plus the legacy generated Projucer/Xcode project.

The legacy generated Xcode files may still reference JUCE modules at:

```text
/Users/hostin/Downloads/JUCE/modules
```

That folder is missing on the current machine. The `.jucer` and CMake build now use:

```text
/Users/hostin/vibecoding/waveform-visualizer/JUCE
```

Override it with `JUCE_PATH=/path/to/JUCE`.

## Canonical Product Naming

Use these names going forward:

- Product: `Buffle Audio Align`
- Short UI name: `Align`
- Future code/project target: `BuffleAlign`
- Preferred bundle ID base: `com.buffleaudio.align`

The CMake path is the canonical build. If the legacy Xcode project is needed, regenerate it from the `.jucer` so generated files pick up the refreshed product name and module path. Avoid hand-editing generated `JuceLibraryCode` as the permanent source of truth.

## CMake Debug Build

```bash
cmake -S . -B build/cmake-debug \
  -DJUCE_PATH=/Users/hostin/vibecoding/waveform-visualizer/JUCE \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build build/cmake-debug --config Debug --parallel
```

## DSP Unit Tests

```bash
cmake -S . -B build/cmake-debug \
  -DJUCE_PATH=/Users/hostin/vibecoding/waveform-visualizer/JUCE \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUFFLE_BUILD_TESTS=ON

cmake --build build/cmake-debug --target BuffleAlignDSPTests --parallel
ctest --test-dir build/cmake-debug --output-on-failure
```

## Release Build And Installer

```bash
scripts/build_and_package_macos.sh
```

Outputs:

```text
dist/stage/Buffle Audio Align.app
dist/stage/Buffle Audio Align.vst3
dist/stage/Buffle Audio Align.component
dist/BuffleAudioAlign-0.3.0-macOS.pkg
```

The bundles are ad-hoc signed for local verification. The package is not Developer ID Installer signed or notarized.

## Legacy Xcode Discovery

```bash
xcodebuild \
  -list \
  -project BufflePlug-Analyzer/Builds/MacOSX/BufflePlug-Analyzer.xcodeproj \
  -derivedDataPath /private/tmp/BuffleAlignDerivedData
```

## Legacy Xcode Build

Run this only after the generated Xcode project is regenerated with a valid JUCE module path:

```bash
xcodebuild \
  -project BufflePlug-Analyzer/Builds/MacOSX/BufflePlug-Analyzer.xcodeproj \
  -scheme "BufflePlug-Analyzer - Shared Code" \
  -configuration Debug \
  -derivedDataPath /private/tmp/BuffleAlignDerivedData \
  build
```

## Recommended Modernization

Move to a pinned JUCE dependency before serious DSP work:

- Preferred: CMake plus `juce_add_plugin`.
- Acceptable short-term: JUCE submodule or vendored `JUCE/` folder.
- Keep VST3, AU, and Standalone first.
- Defer AAX, LV2, and ARA until the core capture/analyze/preview workflow is useful.
