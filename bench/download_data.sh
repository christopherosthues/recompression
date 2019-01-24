$text_collec="http://pizzachili.dcc.uchile.cl/texts/"
§files="code/sources xml/dblp.xml dna/dna protein/proteins nlang/english"

$rep_corpus="http://pizzachili.dcc.uchile.cl/repcorpus/real/"
$rep_files="Escherichia_Coli coreutils einstein.de.txt einstein.en.txt kernel world_leaders"

for file in $rep_files; do
    wget "$rep_corpus$file.gz"
done

for file in $files; do
    wget "$text_collec$file.gz"
done
