# of_mountainScanner
trying to scan visual outlines of landscapes to transform them into sound

## usage

- run openframeworks project via `make RunRelease` or `./bin/of_mountainScanner`
- start Pd via `purr-data -open Pd/mountainScanner.pd -lib mrpeach` (due to some bug, OSC objects don't load without this flag)
- in Pd: start playback of wavetables. (green button in the main window) Make sure "repeat" is activated for the wavetable to be played back repeatedly.
- in of: hit SPACEBAR to start/stop the scanning.
- **image files** must be included in bin/data (hint: create symlink in `bin` to external image folder and call it "data")

## audio setup

Jack needs to be running for purr-data to work. Once Pd is connected to the audio output port in Jack, DSP can be enabled.