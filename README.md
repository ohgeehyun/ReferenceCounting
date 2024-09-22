표준 shared_ptr의 경우에
RefCount 자체는 ThreadSafe하지만, 대입/치환 등 set하는 부분은 그렇지 않다.

그 부분을 꼼꼼히 챙기기 위해서 추가로 Lock을 걸거나그게 귀찮다면 atomic 버전의 shared_ptr가 있지만 성능은 장담 할 수 없음.

표준에 아닌 버전은 어떻게 구현을 했는지가 중요하고 복사를 할때 항상 문제가되는 것이 아닌 "한번에" 복사가 이루어질 수 있는 지가 중요
64비트 운영체제라면 64비트까지가 사실상 원자적으로 복사가 되는 크기 하지만 스마트 포인터에서 원본객체주소(8바이트)+RefCount블록(8바이트)를
동시에 들고 있는 구조라면, 복사가 더 이상 thread-safe하지 않겠죠.
(왜냐하면 정말 8바이트/8바이트를 각각 복사하는 형태로 코드가 만들어지기 때문)
이럴 경우  atomic 버전을 만들거나, lock을 걸거나 별도 처리가 필요

결과적으로 RefCount 관리하는 부분까지 상속받아 객체를 만들기로 합의된 경우, 원본객체주소와 RefCount블록이 사실 동일한 객체가 되어 복사도 안전하지만 일반적인 shared_ptr라면 아니다.
