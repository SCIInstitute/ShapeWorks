import shapeworks as sw
p = sw.Project()
p.load("sample.xlsx")
p.get_headers()
p.get_subjects()
subjects = p.get_subjects()
for s in subjects:
    sub_group_dict = {}
    sub_group_dict["base"] = "yes"
    sub_group_dict["excluded"] = "no"
    s.set_group_values(sub_group_dict)
    print(s.get_display_name())
