#  HerdSoftware examples

## Example 0: creation of Monte Carlo files

Configure and run a Monte Carlo simulation with GGS.

Included files:
* **geometry.mac**  
  Geometry configuration datacard.
* **electrons_10GeV_sphere.mac**  
  Job configuration datacard for generating the Monte Carlo data file to be digitized.
  
  
### Description
This example shows how to create a Monte Carlo file using the `GGSPenny` executable and the Herd parametric geometry.

#### The geomtry configuration datacard
The datacard sets the PSD type to tiles. 

#### The job configuration datacard
The job configuration datacard file `electrons_10GeV_sphere.mac` contains the following sections:
* Setup of the particle generator  
  The example makes use of the GGS gun generator. It is configured to generate 10 GeV electrons with random directions within a
  restricted angular region around the perpendicular vector to the generation surface at generation position. The latter is chosen 
  randomly on a spherical surphace with 160 deg extension and 120 cm radius, centered in the origin of the reference frame.
* Definition of the sensitive volumes  
  The LYSO cubes, Si wafers of the STK and SCD and the scintillating tiles of the PSD are marked as active. For the Si wafers the
  hit type is set to particle hits, since the detector geometry does not implement the single microstrip whose energy deposit must
  then be reconstructed offline by leveraging the information of particle hits.
* Setup of the output information on Root file  
  The information to be written on the Root output file are selected by creating some user actions. In this case the datacard sets
  the detector hits and the Monte Carlo truth information to be saved. For STK and SCD hits also particle hit will be saved since
  they have been activated when defining the sensitive volumes. By default, only particle hits with non-null energy release will be
  saved; it is possible to output also the particle hits with null energy release by lowering the threshold down to a negative
  value (see the commented instructions).
  
* Launch the simulation  
  At the end of the setup the simulation run for 100 events is started.

#### Launch the simulation 
The simulation job can be run with the command:

```bash
$ GGSPenny -g </path/to/HerdCompiledSoftwareFolder>/plugin/libHerdMCParametricGeo.so -d electrons_10GeV_sphere.mac -gd geometry.mac -ro electrons_10GeV_sphere.root
```
where:
</path/to/HerdCompiledSoftwareFolder> is the path to the compiled Herd software (i.e. the 'build' directory or the 'install' one)

This will generate a file called `electrons_10GeV_sphere.root` containing 100 events with a 10 GeV electron primary particle shot from a spherical surface surrounding the detector.

** On Linux the name of the Herd MC geometry shared library is libHerdMCParametricGeo.so, while on OsX is libHerdMCParametricGeo.dylib
