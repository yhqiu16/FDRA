# to the chipyard directory
cd ../../..
# generate verilog file
sbt "project tram; runMain mg.CGRAMG -td ./test_run_dir" -mem 4096

# # generate verilog file
# sbt "runMain dsa.VerilogGen -td ./test_run_dir"
# # generate CGRA top module, no-check-comb-loops
# sbt "runMain dsa.VerilogGen -td ./test_run_dir --no-check-comb-loops"
# # increase JVM heap size, -mem
# sbt "runMain dsa.VerilogGen -td ./test_run_dir --no-check-comb-loops" -mem 4096
# # test
# sbt "test:runMain dsa.CGRATester  -td ./test_run_dir"
# sbt "test:runMain dsa.CGRATester  -td ./test_run_dir -tbn verilator"