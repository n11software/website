import Link from 'next/link'

let ApplyButton = props => {
  return <Link href="/apply">
    <a className="flex cursor-pointer space-x-2 items-center py-2 px-6 rounded-lg transition-colors bg-sky-500 text-gray-200 active:bg-blue-500">
      Apply
    </a>
  </Link>
}
export default ApplyButton