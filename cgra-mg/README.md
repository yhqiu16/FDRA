CGRA-MG
=======================

Template-based CGRA modeling and generation with Chisel

## Getting Started

### Dependencies

#### JDK 8 or newer

#### SBT



### Build and run

Using the script run.sh
```sh
./run.sh
```

Or using sbt command:
```sh
sbt "runMain mg.CGRAMG -td ./test_run_dir"
```

Add -mem option if the JVM heap size is not enough
```sh
sbt "runMain mg.CGRAMG -td ./test_run_dir" -mem 4096
```

#### Generated files

Generated Verilog file: test_run_dir/CGRA.v

Generated architecture IR file: src/main/resource/cgra_adg.json

Generated operation set file: src/main/resource/operations.json

#### Architecture specifications

Optional input file: src/main/resource/cgra_spec.json


