#include "pch.hpp"
#include "ThreadedAlgorithm.hpp"

void ThreadedAlgorithm::ThreadWork::operator()() const {
    //{
    //    std::unique_lock<std::mutex> lock(algorithm->m_mutex);
    //    std::cout << "Thread " << id << " processing " << count << " boids starting at " << start << ".\n";
    //}
    const Boidtree &tree = algorithm->m_tree;
    const Rectangle bounds = algorithm->m_bounds;
    auto &results = algorithm->m_results[id];
    const float disruptive_radius = Boid::disruptiveRadius * Boid::disruptiveRadius;
    const float cohesive_radius = Boid::cohesiveRadius * Boid::cohesiveRadius;

    const Rectangle center_bound {bounds * 0.75f};
    const Rectangle hard_bound {bounds * 0.90f};

    Rectangle search_bound {Vector {Boid::cohesiveRadius}};
    for (ptrdiff_t i = start; i < start + count; ++i) {
        const Boid current = read[i];
        const Boid *previous = read + i;

        search_bound.center = current.position;
        Vector center_steer {0.0f, 0.0f};

        const bool in_center = center_bound.contains(current.position);
        float center_steer_weight = Boid::primadonnaWeight;
        if (!in_center) {
            if (!hard_bound.contains(current.position)) {
                center_steer_weight *= 2.0f;
            }

            center_steer -= current.position;
            center_steer = steer(center_steer, current.velocity);
        }

        const Vector full_speed = steer(current.velocity, current.velocity);

        results.clear();
        search(tree, previous, search_bound, results);
        //std::sort(
        //    results.begin(), results.end(),
        //    [&current](const Boid &a, const Boid &b) {
        //        return glm::distance2(current.position, a.position) < glm::distance2(current.position, b.position);
        //    }
        //);

        Vector separation {0.0f, 0.0f};
        Vector alignment {0.0f, 0.0f};
        Vector cohesion {0.0f, 0.0f};
        size_t cohesive_total = 0;
        size_t disruptive_total = 0;

        for (const Boid &other: results) {
            const float d2 = glm::distance2(current.position, other.position);

            const size_t is_disruptive = d2 < disruptive_radius;
            const size_t is_cohesive = d2 < cohesive_radius;

            separation += FloatEnable[is_disruptive] * ((current.position - other.position) / (d2 + Epsilon));
            alignment += FloatEnable[is_cohesive] * other.velocity;
            cohesion += FloatEnable[is_cohesive] * other.position;

            disruptive_total += is_disruptive;
            cohesive_total += is_cohesive;

            //if (d2 < disruptiveRadius) {
            //    separation += (current.position - other.position) / d2 + epsilon;
            //    disruptive_total++;
            //}

            //if (d2 < cohesiveRadius) {
            //    alignment += other.velocity;
            //    cohesion += other.position;
            //    cohesive_total++;
            //}
        }

        if (disruptive_total > 0) {
            separation /= static_cast<float>(disruptive_total);
            separation = steer(separation, current.velocity);
        }

        if (cohesive_total > 0) {
            const float countFactor = 1.0f / static_cast<float>(cohesive_total);
            alignment *= countFactor;

            cohesion *= countFactor;
            cohesion -= current.position;

            alignment = steer(alignment, current.velocity);
            cohesion = steer(cohesion, current.velocity);
        }

        const Vector acceleration = magnitude(
            Vector {
                center_steer * center_steer_weight
                + full_speed * Boid::speedWeight
                + separation * Boid::separationWeight
                + alignment * Boid::alignmentWeight
                + cohesion * Boid::cohesionWeight
            },
            Boid::maxForce
        );

        write[i].velocity += acceleration;
        write[i].position += current.velocity * delta;
    }
}

ThreadedAlgorithm::ThreadWork::ThreadWork(
    ThreadedAlgorithm *a, int i, float d, const Boid *r, Boid *w, ptrdiff_t c, ptrdiff_t s
) :
    algorithm(a), id(i), delta(d), read(r), write(w), count(c), start(s) {}
