import Link from 'next/link'

let EmployeesPage = props => {
  return props.bool ? <div className="fixed z-0 top-14 left-0 right-0 w-full flex h-full text-gray-800 bg-white">
    <div className="flex flex-col w-1/4 bg-stone-100 p-4">
      <span className="text-md uppercase font-bold mb-8">Founders</span>
      <div className="flex flex-col space-y-2 overflow-y-auto mb-16">
        <Link href="/employees/aristonl">
          <a className="h-18">
            <div className="flex flex-col px-4 py-2 transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">Ariston Lorenzo</span>
              <span className="text-lg text-gray-600 font-light">Developer</span>
            </div>
          </a>
        </Link>
        <Link href="/employees/levih">
          <a className="h-18">
            <div className="flex flex-col px-4 py-2 transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">Levi Hicks</span>
              <span className="text-lg text-gray-600 font-light">Developer</span>
            </div>
          </a>
        </Link>
      </div>
    </div>
    <div className="flex h-full w-full">
      <div className="flex-col h-full w-full">
        <div className="flex flex-col w-full pt-8 pl-8 pb-2">
          <Link href="/employees/filter/developers">
            <a className="font-extrabold text-xl">
              Developers:
            </a>
          </Link>
        </div>
        <div className="flex flex-col h-full pl-8 space-y-2 overflow-y-scroll">
          <Link href="/employees/aristonl">
            <a className="flex flex-col px-4 py-2 w-48 h-10 truncate justify-center transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">Ariston Lorenzo</span>
            </a>
          </Link>
          <Link href="/employees/levih">
            <a className="flex flex-col px-4 py-2 w-48 h-10 justify-center transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">Levi Hicks</span>
            </a>
          </Link>
          <Link href="/employees/nolang">
            <a className="flex flex-col px-4 py-2 w-48 h-10 justify-center transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">Nolan Gerold</span>
            </a>
          </Link>
          <Link href="/employees/genesisb">
            <a className="flex flex-col px-4 py-2 w-48 h-10 justify-center transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">Genesis Banzon</span>
            </a>
          </Link>
          <Link href="/employees/joshuat">
            <a className="flex flex-col px-4 py-2 w-48 h-10 justify-center transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">Joshua Torres</span>
            </a>
          </Link>
          <Link href="/employees/jamesv">
            <a className="flex flex-col px-4 py-2 w-48 h-10 justify-center truncate transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">James Villanueva</span>
            </a>
          </Link>
        </div>
      </div>
      <div className="flex-col h-full w-full">
        <div className="flex flex-col w-full pt-8 pl-8 pb-2">
          <Link href="/employees/filter/developers">
            <a className="font-extrabold text-xl">
              Graphic Designers:
            </a>
          </Link>
        </div>
        <div className="flex flex-col h-full pl-8 space-y-2 overflow-y-scroll">
          <Link href="/employees/kevink">
            <a className="flex flex-col px-4 py-2 w-48 h-10 justify-center transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">Kevin kennedy</span>
            </a>
          </Link>
          <Link href="/employees/joshuat">
            <a className="flex flex-col px-4 py-2 w-48 h-10 justify-center transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">Joshua Torres</span>
            </a>
          </Link>
          <Link href="/employees/jamesv">
            <a className="flex flex-col px-4 py-2 w-48 h-10 truncate justify-center transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">James Villanueva</span>
            </a>
          </Link>
        </div>
      </div>
      <div className="flex-col h-full w-full">
        <div className="flex flex-col w-full pt-8 pl-8 pb-2">
          <Link href="/employees/filter/developers">
            <a className="font-extrabold text-xl">
              Sound Design:
            </a>
          </Link>
        </div>
        <div className="flex flex-col h-full pl-8 space-y-2 overflow-y-scroll">
          <Link href="/employees/genesisb">
            <a className="flex flex-col px-4 py-2 w-48 h-10 justify-center transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">Genesis Banzon</span>
            </a>
          </Link>
        </div>
      </div>
      <div className="flex-col h-full w-full">
        <div className="flex flex-col w-full pt-8 pl-8 pb-2">
          <Link href="/employees/filter/developers">
            <a className="font-extrabold text-xl">
              Writers:
            </a>
          </Link>
        </div>
        <div className="flex flex-col h-full pl-8 space-y-2 overflow-y-scroll">
          <Link href="/employees/williaml">
            <a className="flex flex-col px-4 py-2 w-48 h-10 justify-center truncate transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">William Lavender</span>
            </a>
          </Link>
          <Link href="/employees/aleksl">
            <a className="flex flex-col px-4 py-2 w-48 h-10 justify-center transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">Aleks Londoneer</span>
            </a>
          </Link>
        </div>
      </div>
    </div>
  </div>: <></>
}

export default EmployeesPage