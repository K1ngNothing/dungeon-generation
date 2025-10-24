# Global Tasks:
- [ ] Algorithm for fixing solution imperfections.
  - [ ] Splitting corridor in half if it going through the room 
  - [ ] Routing (much harder)
- [ ] Implement proper multi-threading.
  - [ ] Bigget roadblock: it seems that PETSc TAO only utilizes MPI parallelization, which I'm not familiar with. My code isn't a bottleneck yet.  

# Smaller Tasks:
- [ ] Add coordinates normalization? Because for now contraints are bounded, but target function is not.

# What to research:
- PETSc parallelization tutorial: [link](https://petsc.org/release/manual/getting_started/#parallel-and-gpu-programming)
- Books or tutorials for MPI? Maybe useful outside this project.
- Guides on writing code for GPU (for example, this course about algorithms on GPU: [lectures](https://www.youtube.com/playlist?list=PLlb7e2G7aSpSptbl_yI5uvMlpRc1mwsCL), [repo](https://github.com/DanielGabitov/GPGPUTasks2022)) 
