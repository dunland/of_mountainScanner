# of_mountainScanner
trying to scan visual outlines of landscapes to transform them into sound

## quickstart

- run openframeworks project via `make RunRelease` or `./bin/of_mountainScanner`
- start Pd via `purr-data -open Pd/mountainScanner.pd -lib mrpeach` (due to some bug, OSC objects don't load without this flag)
- in Pd: start playback of wavetables. (green button in the main window) Make sure "repeat" is activated for the wavetable to be played back repeatedly.
- in of: hit SPACEBAR to start/stop the scanning.

## usage

openframeworks will load all images in the bin/data folder (not recursively)

### data

openframeworks exports csv files containing an array of white pixels with highest y value (mapped to -1/1 range) for each image to `bin/data`. This data can be analyzed using the python script `data_processing`. This will read all the data and plot it. It also translates the data to 16-bit and 256 samples to be handled by the `Frantasia` synth.

## audio setup

Jack needs to be running for purr-data to work. Once Pd is connected to the audio output port in Jack, DSP can be enabled.