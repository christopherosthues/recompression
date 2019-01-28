text_collec="http://pizzachili.dcc.uchile.cl/texts/"
files="code/sources xml/dblp.xml dna/dna protein/proteins nlang/english"
file_names="sources dblp.xml dna proteins english"

rep_corpus="http://pizzachili.dcc.uchile.cl/repcorpus/real/"
rep_files="Escherichia_Coli coreutils einstein.de.txt einstein.en.txt kernel world_leaders"

for file in $rep_files; do
    wget "$rep_corpus$file.gz"
done
for file_name in $file_names; do
    gunzip $file_name
done

for file in $files; do
    wget "$text_collec$file.gz"
    gunzip $file
done
