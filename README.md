# SFIND-SOPE

Grupo 2 Turma 5


## TODO

Após leitura do enunciado o que falta fazer:

->Refactoring total :D ( Ja comecei e acho que já está com grandes progressos)
 
->No gerador passar o fifo para /tmp/entrada, /tmp/rejeitados e /tmp/ger.pid (done)

->No gerador o instante no ficheiro converter para milisegundos e com 2 casas decimais

->Na sauna passar o fifo para /tmp/entrada, /tmp/rejeitados e /tmp/bal.pid (done)

-> Na sauna imprimir as estatisticas no final do ficheiro

-> Na sauna o instante no ficheiro converter para milisegundos e com 2 casas decimais

->Testar (É preciso testar exaustivamente, porque após refactoring não se fiz asneira. Em aleatoriadade so encontrei difrenças acima dos 1000 pedidos, pq os outros ficaram me iguais :) )

->E fazer verificaçes de reads, create fifos and so on... com devivo controlo de erros (Acho que esta parte já está, falta verificar os prints dos erros (mistura com portugues e ingles)

Depois a parte:

->um ficheiro de texto, identificando os elementos do grupo de alunos autores e contendo uma explicação sucinta de como foram evitadas no código desenvolvido as situações de competição (race conditions) no acesso a elementos partilhados.


->dois ficheiros de texto, cada um contendo toda a informação respeitante à execução de uma instância de gerador e de 
sauna (comando de invocação, informação da saída padrão e registos).

Sugestões by Damas:
Usar um semaforo nos request com threads em vez do contador

a cada request aceite é só dar sem wait, usleep, sem post 🙂

