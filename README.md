ftalat : Frequency Transition Latency Estimator

ftalat allows you to determine the time taken by your CPU to switch from one frequency to another one.

# Compilation
```
    make
```
or
```
    make trace
```
to compile a second version of ftalat writing latency for each run of the kernel. A graph of this data can be displayed using the GNUPlot script `traceViewer.gnuplot`

# Usage
```
    ./ftalat startFreq targetFreq
    where startFreq is the frequency at the beginning of the test and targetFreq the frequency to switch to
    The program will output the time taken by your CPU to swtich from startFreq to targetFreq
    ftalat must be run with enough permissions to access cpufreq files
```

The script `ftalat_runner.sh` allows you to run the test for all frequencies couple for your CPU. The script will write the output in a folder `Results`
From the files written in the `Results` folder, you can generate graphs. To do this, you need to use the python scripts from the `script` folder.
```
    ./extract.py data_folder output_folder
    data_folder should be 'Results' folder
    output_folder is a new folder (which has to be created before) where the proceed files will be stored.
```

Then, you have to run
```
    ./generateGraph.py input_folder file
    where input_folder is the folder where the files generated by extract.py are located
    and file, the name of the file (without extension) for graph output (PDF) and latency table (a GNUPlot script is also generated)
```

# Depencies
You must have python (2.7) and GNUPlot installed

# Licence
The program is licenced under GPLv3. Please read [COPYRIGHT](https://github.com/LittleWhite-tb/ftalat/blob/master/COPYRIGHT) file for more information