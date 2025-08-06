## Issues:
- [ ] `generateTree` generates layout that can't be placed on 2D plane. Example:
<p align="center">
<img src="images/tree_25_problem.png" alt="drawing" width="400"/>
</p>
Solution: limit adjacent vertices count to 3-4? Or maybe it won't be a problem with proper doors, because we won't have multiple corridors connected to the same door.

- [ ] Unstable results, even though I've seeded every random event (or maybe they are stable? Because I've got the same result for tree layout with 100 vertices as yesterday).

## TODO:
- [x] Create a generator with proper doors. For now it's fine to have a tree graph structure.
- [x] Add unit-tests. Especially I must test correctness of my functions and their gradients.
- [x] Implement anti-density function that pushes disconnected rooms away from each other. Maybe this will lower count of "bad corridors"?
  * Notes on that: for now the function is $$\frac{scale}{2 \cdot \frac{x_1 - x_2}{range \cdot (w_1 + w_2)} + 2 \cdot \frac{y_1 - y_2}{range \cdot (h_1 + h_2)} + 1}$$ Point is that this function lies in range $[0; scale]$ and $range$ is responsible for the point where function reaches half of it's value (e.g. with $range = 1$ value of the first fraction reaches $0.5$ when rooms are touching on the $x$ axis).
  * Ideally we need a coordinate normalization, because this function is bounded, but corridor length isn't
- [ ] Test movable doors. This might allow us to no longer worry about a feasibility of the solution, and allow us to just set a desired graph.
- [ ] Add coordinates normalization? Don't really now is it relevant in this task or not, but maybe worth implementing and testing.
- [ ] Algorithm for fixing solution imperfections? Maybe remove some edges and add new ones?

