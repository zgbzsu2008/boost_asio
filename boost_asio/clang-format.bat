for %%f in (*.h *.cpp *.hpp *.cc) do (
clang-format.exe -i -style=file %%f
)

for %%f in (*.h) do (
clang-format.exe -i -style=file %%f
perl -p -i.bak -e "s/public:/public:\n/g" %%f
perl -p -i.bak -e "s/protected:/protected:\n/g" %%f
perl -p -i.bak -e "s/private:/private:\n/g" %%f
)

del *.bak
del *.tmp