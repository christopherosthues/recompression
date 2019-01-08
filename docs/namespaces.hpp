#pragma once

/**
 * @brief Contains the recompression technique.
 *
 * This is the central namespace in which the recompression and all necessary utilities are contained.
 */
namespace recomp {

/**
 * @brief Contains LCE query algorithms.
 */
namespace lce_query {}


/**
 * @brief Contains all parallelized version of the algorithms like the recompression itself or several versions of parallel radix sort.
 */
namespace parallel {}

/**
 * @brief Contains utilities needed for the recompression, io etc.
 */
namespace util {}

}
