#include "ExcelProjectWriter.h"

#include <xlnt/workbook/workbook_view.hpp>
#include <xlnt/xlnt.hpp>

#include "ProjectUtils.h"

namespace shapeworks {

//---------------------------------------------------------------------------
static int get_index_for_column(xlnt::worksheet& ws, const std::string& name) {
  auto headers = ws.rows(false)[0];
  // std::cerr << "Get index for column: " << name << "\n";
  // std::cerr << "number of headers = " << headers.length() << "\n";
  for (int i = 0; i < headers.length(); i++) {
    // std::cerr << "header: " << headers[i].to_string() << "\n";
    if (headers[i].to_string() == name) {
      return i + 1;
    }
  }

  // std::cerr << "couldn't find: " << name << "\n";
  auto column = ws.highest_column();
  // std::cerr << "highest column is " << column.index << "\n";
  if (ws.cell(xlnt::cell_reference(column.index, 1)).value<std::string>().empty()) {
    ws.cell(xlnt::cell_reference(column.index, 1)).value(name);
    // std::cerr << "returning " << column.index << "\n";
    return column.index;
  } else {
    ws.cell(xlnt::cell_reference(column.index + 1, 1)).value(name);
    // std::cerr << "returning " << column.index + 1 << "\n";
    return column.index + 1;
  }

  return -1;
}

//---------------------------------------------------------------------------
static void set_value(xlnt::worksheet& ws, int column, int subject_id, const std::string& value) {
  ws.cell(xlnt::cell_reference(column, subject_id)).value(value);
}

//---------------------------------------------------------------------------
static void set_value(xlnt::worksheet& ws, const std::string& column_name, int subject_id, const std::string& value) {
  int column_index = get_index_for_column(ws, column_name);
  set_value(ws, column_index, subject_id + 2, value);  // +1 for header, +1 for 1-indexed
}

//---------------------------------------------------------------------------
static void assign_keys(xlnt::worksheet& ws, int subject_id, std::vector<std::string> prefixes,
                        std::vector<std::string> filenames, std::vector<std::string> domains) {
  assert(!prefixes.empty());
  assert(!domains.empty());
  if (prefixes.empty()) {
    throw std::runtime_error("Empty prefixes");
  }
  if (domains.empty()) {
    throw std::runtime_error("Empty domains");
  }
  if (filenames.empty()) {
    return;
  }
  auto prefix = prefixes[0];
  if (filenames.size() != domains.size()) {
    throw std::runtime_error(prefix + " filenames and number of domains mismatch");
  }
  for (int i = 0; i < domains.size(); i++) {
    if (prefixes.size() == domains.size()) {
      prefix = prefixes[i];
    }
    std::string key = prefix + "_" + domains[i];
    std::string value = filenames[i];
    set_value(ws, key, subject_id, value);
  }
}

//---------------------------------------------------------------------------
static std::string transform_to_string(std::vector<double> transform) {
  std::string str;
  for (int j = 0; j < transform.size(); j++) {
    if (j == 0) {
      str = std::to_string(transform[j]);
    } else {
      str = str + " " + std::to_string(transform[j]);
    }
  }
  return str;
}

//---------------------------------------------------------------------------
static void assign_transforms(xlnt::worksheet& ws, int subject_id, std::string prefix,
                              std::vector<std::vector<double>> transforms, std::vector<std::string> domains) {
  if (transforms.empty()) {
    return;
  }
  if (transforms.size() != domains.size() && transforms.size() != domains.size() + 1) {
    throw std::runtime_error(prefix + " filenames and number of domains mismatch");
  }
  for (int i = 0; i < domains.size(); i++) {
    std::string key = prefix + "_" + domains[i];
    std::string value = transform_to_string(transforms[i]);
    set_value(ws, key, subject_id, value);
  }
}

//---------------------------------------------------------------------------
static void store_subjects(Project& project, xlnt::workbook& wb) {
  auto subjects = project.get_subjects();
  auto domains = project.get_domain_names();

  xlnt::worksheet ws = wb.sheet_by_index(0);
  ws.title("data");

  for (int i = 0; i < subjects.size(); i++) {
    auto subject = subjects[i];
    set_value(ws, "name", i, subject->get_display_name());
    auto original_prefixes = ProjectUtils::convert_domain_types(project.get_original_domain_types());
    auto groomed_prefixes = ProjectUtils::convert_groomed_domain_types(project.get_groomed_domain_types());
    assign_keys(ws, i, original_prefixes, subject->get_original_filenames(), domains);
    assign_keys(ws, i, {"landmarks_file"}, subject->get_landmarks_filenames(), domains);
    assign_keys(ws, i, {"constraints"}, subject->get_constraints_filenames(), domains);
    assign_keys(ws, i, groomed_prefixes, subject->get_groomed_filenames(), domains);
    assign_keys(ws, i, {"local_particles"}, subject->get_local_particle_filenames(), domains);
    assign_keys(ws, i, {"world_particles"}, subject->get_world_particle_filenames(), domains);
    assign_transforms(ws, i, {"alignment"}, subject->get_groomed_transforms(), domains);
    assign_transforms(ws, i, {"procrustes"}, subject->get_procrustes_transforms(), domains);

    // write out extra values
    for (auto& [key, value] : subject->get_extra_values()) {
      set_value(ws, key, i, value);
    }
  }
}

//---------------------------------------------------------------------------
static int get_or_create_worksheet(xlnt::workbook& wb, std::string name) {
  if (wb.contains(name)) {
    auto ws = wb.sheet_by_title(name);
    return wb.index(ws);
  } else {
    auto ws = wb.create_sheet();
    ws.title(name);
    return wb.index(ws);
  }
}

//---------------------------------------------------------------------------
static void create_parameter_sheet(xlnt::workbook& wb, std::string name, Parameters params,
                                   std::string domain_name = "") {
  int id = get_or_create_worksheet(wb, name);
  auto ws = wb.sheet_by_index(id);
  ws.cell(xlnt::cell_reference(1, 1)).value("key");
  ws.cell(xlnt::cell_reference(2, 1)).value("value");

  int key_column = 1;
  int value_column = 2;

  if (domain_name == "") {
    ws.cell(xlnt::cell_reference(1, 1)).value("key");
    ws.cell(xlnt::cell_reference(2, 1)).value("value");
  } else {
    key_column = get_index_for_column(ws, "key");
    value_column = get_index_for_column(ws, "value_" + domain_name);
  }

  int row = 2;
  for (const auto& kv : params.get_map()) {
    ws.cell(xlnt::cell_reference(key_column, row)).value(kv.first);
    ws.cell(xlnt::cell_reference(value_column, row)).value(kv.second);
    row++;
  }
}

//---------------------------------------------------------------------------
static void create_parameter_map_sheet(xlnt::workbook& wb, std::string name,
                                       std::map<std::string, Parameters> parameter_map) {
  for (auto& [key, params] : parameter_map) {
    create_parameter_sheet(wb, name, params, key);
  }
}

//---------------------------------------------------------------------------
static void store_landmark_definitions(Project& project, xlnt::workbook& wb) {
  std::string name = "landmarks";
  if (wb.contains(name)) {
    wb.remove_sheet(wb.sheet_by_title(name));
  }

  int id = get_or_create_worksheet(wb, name);
  auto ws = wb.sheet_by_index(id);

  auto domain_names = project.get_domain_names();

  ws.cell(xlnt::cell_reference(1, 1)).value("domain");
  ws.cell(xlnt::cell_reference(2, 1)).value("name");
  ws.cell(xlnt::cell_reference(3, 1)).value("visible");
  ws.cell(xlnt::cell_reference(4, 1)).value("color");
  ws.cell(xlnt::cell_reference(5, 1)).value("comment");

  auto all_definitions = project.get_all_landmark_definitions();

  int row = 2;
  for (int domain_id = 0; domain_id < all_definitions.size(); domain_id++) {
    auto landmarks = all_definitions[domain_id];
    for (int i = 0; i < landmarks.size(); i++) {
      ws.cell(xlnt::cell_reference(1, row)).value(domain_names[domain_id]);
      ws.cell(xlnt::cell_reference(2, row)).value(landmarks[i].name_);
      ws.cell(xlnt::cell_reference(3, row)).value(landmarks[i].visible_ ? "true" : "false");
      ws.cell(xlnt::cell_reference(4, row)).value(landmarks[i].color_);
      ws.cell(xlnt::cell_reference(5, row)).value(landmarks[i].comment_);
      row++;
    }
  }
}

//---------------------------------------------------------------------------
bool ExcelProjectWriter::write_project(Project& project, std::string filename) {
  auto wb = xlnt::workbook{};

  store_subjects(project, wb);

  create_parameter_map_sheet(wb, "groom", project.get_parameter_map("groom"));

  create_parameter_sheet(wb, "optimize", project.get_parameters("optimize"));
  create_parameter_sheet(wb, "studio", project.get_parameters("studio"));
  create_parameter_sheet(wb, "project", project.get_parameters("project"));
  create_parameter_sheet(wb, "analysis", project.get_parameters("analysis"));
  create_parameter_sheet(wb, "deepssm", project.get_parameters("deepssm"));

  store_landmark_definitions(project, wb);

  wb.save(filename);

  return true;
}

}  // namespace shapeworks
