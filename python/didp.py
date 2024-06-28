import read2didp
import didppy as dp
import time
import sys

if __name__ == '__main__':

    if len(sys.argv) != 2:
        print("Usage: python script.py input_file")
        sys.exit(1)
    
    file_path = sys.argv[1]

    # Read in the data

    n, a, b, c = read2didp.read_data(file_path)

    # Modeling

    model = dp.Model()

    customer = model.add_object_type(number=n)

    # U
    unvisited = model.add_set_var(object_type=customer, target=list(range(1, n)))
    # i
    location = model.add_element_var(object_type=customer, target=0)
    # t
    time = model.add_int_resource_var(target=0, less_is_better=True)

    travel_time = model.add_int_table(c)

    for j in range(1, n):
        visit = dp.Transition(
            name="visit {}".format(j),
            cost=travel_time[location, j] + dp.IntExpr.state_cost(),
            preconditions=[unvisited.contains(j)],
            effects=[
                (unvisited, unvisited.remove(j)),
                (location, j),
                (time, dp.max(time + travel_time[location, j], a[j])),
            ],
        )
        model.add_transition(visit)

    return_to_depot = dp.Transition(
        name="return",
        cost=travel_time[location, 0] + dp.IntExpr.state_cost(),
        effects=[
            (location, 0),
            (time, time + travel_time[location, 0]),
        ],
        preconditions=[unvisited.is_empty(), location != 0],
    )
    model.add_transition(return_to_depot)

    model.add_base_case([unvisited.is_empty(), location == 0])

    for j in range(1, n):
        model.add_state_constr(
            ~unvisited.contains(j) | (time + travel_time[location, j] <= b[j])
        )

    min_to = model.add_int_table(
        [min(c[k][j] for k in range(n) if k != j) for j in range(n)]
    )

    model.add_dual_bound(min_to[unvisited] + (location != 0).if_then_else(min_to[0], 0))

    min_from = model.add_int_table(
        [min(c[j][k] for k in range(n) if k != j) for j in range(n)]
    )

    model.add_dual_bound(
        min_from[unvisited] + (location != 0).if_then_else(min_from[location], 0)
    )

    # Solving

    import time

    solver = dp.CABS(model, quiet=True)
    start_time = time.time()
    solution = solver.search()
    end_time = time.time()
    elapsed_time = (end_time - start_time) * 1000

    print("Transitions to apply:")
    print("")

    for t in solution.transitions:
        print(t.name)

    print("")
    print("Cost: {}".format(solution.cost))
    print("Runtime: {} ms".format(elapsed_time))