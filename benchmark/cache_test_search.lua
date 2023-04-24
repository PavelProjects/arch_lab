local frandom = io.open("/dev/urandom", "rb")
local d = frandom:read(4)
math.randomseed(d:byte(1) + (d:byte(2) * 256) + (d:byte(3) * 65536) + (d:byte(4) * 4294967296))

number =  math.random(1,100000)
request = function()
    headers = {}
    headers["Content-Type"] = "application/json"
    headers["Authorization"] = "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE2ODIzNTI2NzIuNjU3MTUxLCJpZCI6MTAwNSwibG9naW4iOiJhdXRvdGVzdF9hZG1pbiIsInN1YiI6ImxvZ2luIn0.EL_m2riiHpoLCg9wruSVjhpzi_4be_k9OM8XTAR7Rq4"
    body = ''
    return wrk.format("GET", "/user/search?name=test".. tostring(number), headers, body)
end