import shapeworks as sw
p = sw.Project()
p.load("sample.xlsx")
print(p.get_headers())

optim_params = p.get_parameters("optimize").as_map()
for param in optim_params:
    print(f'{param} : {optim_params[param]}')

print(sw.Variant("10").as_str())

print(p.get_parameters("project").as_map()["version"])
project_params = p.get_parameters("project")
project_params.set("version", sw.Variant("3"))
print(project_params.get("version", sw.Variant("10")).as_str())
print(project_params.as_map()["version"])

subjects = p.get_subjects()
for s in subjects:
    sub_group_dict = {}
    sub_group_dict["base"] = "yes"
    sub_group_dict["excluded"] = "no"
    s.set_group_values(sub_group_dict)
    print(s.get_display_name())
