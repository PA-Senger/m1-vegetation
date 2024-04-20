#include "../include/config.hpp"
#include "../include/json_helpers.hpp"
#include "../include/query.hpp"
#include "../include/tree.hpp"
// #include <cpr/cpr.h>
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv) {

    Config config("../config.json");

    std::cout << "A_lat: " << config.getAlat() << std::endl;
    std::cout << "A_lon: " << config.getAlon() << std::endl;
    std::cout << "B_lat: " << config.getBlat() << std::endl;
    std::cout << "B_lon: " << config.getBlon() << std::endl;

    auto query = config.get_query();

    query.perform_query();
    nlohmann::json jsonData = query.get_query_result();

    // Generate tree meshes
    auto treeLibrary = createLibraryFromJson(jsonData);

    for (auto &tree : treeLibrary) {
        std::cout << "Tree ID: " << tree.getId() << std::endl;
        std::cout << "Tree Lat: " << tree.getLat() << std::endl;
        std::cout << "Tree Lon: " << tree.getLon() << std::endl;
        std::cout << "Tree Genus: " << tree.getGenus() << std::endl;
        std::cout << "Tree Species: " << tree.getSpecies() << std::endl;
        std::cout << "Tree Height: " << tree.getHeight() << std::endl;
        std::cout << "Tree Circumference: " << tree.getCircumference()
                  << std::endl;
        std::cout << "Tree Diameter Crown: " << tree.getDiameterCrown()
                  << std::endl;
    }

    // Mesh part
    ///////////////////////////////////////////////////////////////

    Mesh finalMesh;
    Mesh currentWrap;
    double ref_lat = config.getAlat();
    double ref_lon = config.getAlon();

    int i = 0;
    for (auto &tree : treeLibrary) {
        tree.computeXY(ref_lat, ref_lon);
        // std::cout << "Tree X: " << tree.getX() << std::endl;
        // std::cout << "Tree Y: " << tree.getY() << std::endl;

        tree.wrap(0);
        currentWrap = tree.getWrap();

        // Compute the union of the meshes
        if (!CGAL::Polygon_mesh_processing::corefine_and_compute_union(
                finalMesh, currentWrap, finalMesh)) {
            std::cerr << "Corefine_and_compute_union failed." << std::endl;
            return 1;
        }

        // std::string name = "../output/tree" + std::to_string(i) + ".off";
        // CGAL::IO::write_OFF(name, currentWrap);
        // i++;
    }

    // Write the resulting mesh to an OFF file
    std::ofstream output("../output/union.off");
    CGAL::IO::write_OFF(output, finalMesh);

    return 0;
}

// #include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
// #include <CGAL/IO/polygon_soup_io.h>
// #include <CGAL/Polygon_mesh_processing/bbox.h>
// #include <CGAL/Real_timer.h>
// #include <CGAL/Surface_mesh.h>
// #include <CGAL/alpha_wrap_3.h>
// #include <array>
// #include <iostream>
// #include <string>
// #include <vector>
// namespace AW3 = CGAL::Alpha_wraps_3;
// using K = CGAL::Exact_predicates_inexact_constructions_kernel;
// using Point_3 = K::Point_3;
// using Mesh = CGAL::Surface_mesh<Point_3>;
// int main(int argc, char **argv) {
//     std::cout.precision(17);
//     // Read the input
//     const std::string filename =
//         (argc > 1) ? argv[1]
//                    : CGAL::data_file_path("meshes/blobby-shuffled.off");
//     std::cout << "Reading " << filename << "..." << std::endl;
//     std::vector<Point_3> points;
//     std::vector<std::array<std::size_t, 3>> faces;
//     if (!CGAL::IO::read_polygon_soup(filename, points, faces) ||
//         faces.empty()) {
//         std::cerr << "Invalid input." << std::endl;
//         return EXIT_FAILURE;
//     }
//     std::cout << "Input: " << points.size() << " points, " << faces.size()
//               << " faces" << std::endl;
//     // Compute the alpha and offset values
//     const double relative_alpha = (argc > 2) ? std::stod(argv[2]) : 20.;
//     const double relative_offset = (argc > 3) ? std::stod(argv[3]) : 600.;
//     CGAL::Bbox_3 bbox;
//     for (const Point_3 &p : points)
//         bbox += p.bbox();
//     const double diag_length =
//         std::sqrt(CGAL::square(bbox.xmax() - bbox.xmin()) +
//                   CGAL::square(bbox.ymax() - bbox.ymin()) +
//                   CGAL::square(bbox.zmax() - bbox.zmin()));
//     const double alpha = diag_length / relative_alpha;
//     const double offset = diag_length / relative_offset;
//     // Construct the wrap
//     CGAL::Real_timer t;
//     t.start();
//     Mesh wrap;

//     double scaling_factor = 10 / (bbox.zmax() - bbox.zmin());
//     for (auto &point : points) {
//         point = Point_3(point.x() * scaling_factor, point.y() *
//         scaling_factor,
//                         point.z() * scaling_factor);
//     }

//     CGAL::alpha_wrap_3(points, faces, alpha, offset, wrap);
//     t.stop();
//     std::cout << "Result: " << num_vertices(wrap) << " vertices, "
//               << num_faces(wrap) << " faces" << std::endl;
//     std::cout << "Took " << t.time() << " s." << std::endl;
//     // Save the result
//     std::string input_name = std::string(filename);
//     input_name = input_name.substr(input_name.find_last_of("/") + 1,
//                                    input_name.length() - 1);
//     input_name = input_name.substr(0, input_name.find_last_of("."));
//     std::string output_name =
//         input_name + "_" + std::to_string(static_cast<int>(relative_alpha)) +
//         "_" + std::to_string(static_cast<int>(relative_offset)) + ".stl";
//     std::cout << "Writing to " << output_name << std::endl;
//     CGAL::IO::write_polygon_mesh(output_name, wrap,
//                                  CGAL::parameters::stream_precision(17));
//     return EXIT_SUCCESS;
// }