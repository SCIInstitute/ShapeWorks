import shapeworks as sw
p = sw.Project()
p.load("sample.xlsx")
p.get_headers()
p.get_subjects()
subjects = p.get_subjects()
for s in subjects:
    print(s.get_display_name())
